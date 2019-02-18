#ifndef puzzler_puzzles_rank_hpp
#define puzzler_puzzles_rank_hpp

#include <random>
#include <sstream>
#include <vector>
#include <climits>
#include <cassert>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{

  class RankPuzzle;
  class RankInput;
  class RankOutput;

  class RankInput
    : public Puzzle::Input
  {
  public:
    std::vector<std::vector<uint32_t> > edges;
    float tol;
    
    RankInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    RankInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override final
    {
      conn.SendOrRecv(edges);
      conn.SendOrRecv(tol);
    }
  };

  class RankOutput
    : public Puzzle::Output
  {
  public:
    std::vector<float> ranks;

    RankOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    RankOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override
    {
      conn.SendOrRecv(ranks);
    }

  };

  class RankPuzzle
    : public PuzzleBase<RankInput,RankOutput>
  {
  protected:
    float norm(const std::vector<float> &a, const std::vector<float> &b) const
    {
      double acc=0;
      for(unsigned i=0; i<a.size(); i++){
        acc += pow(a[i]-b[i],2.0);
      }
      return sqrt(acc);
    }

    void iteration(ILog *log, unsigned n, const std::vector<std::vector<uint32_t> > &edges, const float *current, float *next) const
    {
      for(unsigned i=0; i<n; i++){
        next[i]=0;
      }
      for(unsigned i=0; i<n; i++){
        for(unsigned j=0; j<edges[i].size(); j++){
          int dst=edges[i][j];
          next[dst] += current[i] / edges[i].size();
        }
      }

      double total=0;
      for(unsigned i=0; i<n; i++){
        next[i] = (current[i] * 0.3  + next[i] * 0.7 );
        total += next[i];
      }
      log->LogVerbose("  total=%g", total);
      for(unsigned i=0; i<n; i++){
        next[i] /= total;
        log->LogVerbose("    c[%u] = %g", i, next[i]);
      }
    }

    void ReferenceExecute(
			  ILog *log,
			  const RankInput *pInput,
			  RankOutput *pOutput
			  ) const
    {
      const std::vector<std::vector<uint32_t> > &edges=pInput->edges;
      float tol=pInput->tol;
      unsigned n=edges.size();

      log->LogInfo("Starting iterations.");
      std::vector<float> curr(n, 0.0f);
      curr[0]=1.0;
      std::vector<float> next(n, 0.0f);
      float dist=norm(curr,next);
      while( tol < dist ){
        log->LogVerbose("dist=%g", dist);
        iteration(log, n, edges, &curr[0], &next[0]);
        std::swap(curr, next);
        dist=norm(curr, next);
      }
      
      pOutput->ranks=curr;
      
      log->LogInfo("Finished");
    }

    virtual void Execute(
      ILog *log,
      const RankInput *input,
      RankOutput *output
      ) const {
      log->LogInfo("Using ReferenceExecute to implement Execute. Should this be overriden?");
      ReferenceExecute(log, input, output); 
    }

  public:
    virtual std::string Name() const override
    { return "rank"; }

    virtual bool HasBitExactOutput() const override
    { return false; }

    virtual bool CompareOutputs(ILog * /*log*/, const RankInput * input, const RankOutput *ref, const RankOutput *got) const override
    {
      return norm(ref->ranks, got->ranks) < sqrt(ref->ranks.size()) * input->tol;
    }

    virtual std::shared_ptr<Input> CreateInput(
					       ILog * /*log*/,
					       int scale
					       ) const override
    {
      std::mt19937 rnd(time(0));  // Not the best way of seeding...
      if(getenv("DT10_DET_SEED")){
        int seed=atoi(getenv("DT10_DET_SEED"));
        rnd.seed(seed);
      }

      std::uniform_real_distribution<> udist;
      
      auto params=std::make_shared<RankInput>(this, scale);

      unsigned n=scale;
      params->tol=2e-8;

      unsigned degree=ceil(2+pow(n,0.2));

      params->edges.resize(n);
      for(unsigned i=0; i<n; i++){
        //std::cerr<<" "<<i<<" : ";
        params->edges[i].push_back( (i+1)%n );
        for(unsigned j=1; j<degree; j++){
          params->edges[i].push_back( rnd()%n );
          //std::cerr<<params->edges[i].back()<<" ";
        }
        //std::cerr<<"\n";
      }
      
      return params;
    }

  };

};

#endif
