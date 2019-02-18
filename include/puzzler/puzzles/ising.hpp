#ifndef puzzler_puzzles_ising_hpp
#define puzzler_puzzles_ising_hpp

#include <random>
#include <sstream>
#include <vector>
#include <climits>
#include <cassert>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{

  class IsingPuzzle;
  class IsingInput;
  class IsingOutput;

  class IsingInput
    : public Puzzle::Input
  {
  public:
    uint32_t n;
    uint32_t seed;
    uint32_t prob;
    
    IsingInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    IsingInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override final
    {
      conn.SendOrRecv(n);
      conn.SendOrRecv(seed);
      conn.SendOrRecv(prob);
    }
  };

  class IsingOutput
    : public Puzzle::Output
  {
  public:
    std::vector<unsigned> history;

    IsingOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    IsingOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override
    {
      conn.SendOrRecv(history);
    }

  };

  class IsingPuzzle
    : public PuzzleBase<IsingInput,IsingOutput>
  {
  protected:
    enum {
      rng_group_bond_lr=1,
      rng_group_bond_ud=2,
      rng_group_flip=3,
      rng_group_init=4,
    };

    uint32_t hround(uint32_t acc, uint32_t data) const
    {
      acc += data * 2246822519U;
      acc  = (acc<<13) | (acc>>(32-13));
      return acc * 2654435761U;
    }

    uint32_t hfinal(uint32_t acc) const
    {
      acc ^= acc >> 15;
      acc *= 2246822519U;
      acc ^= acc >> 13;
      acc *= 3266489917U;
      acc ^= acc >> 16;
      return acc;
    }

    uint32_t hrng(uint32_t seed, uint32_t group, uint32_t iter, uint32_t pos) const
    {
      uint32_t acc=0;
      acc=hround(acc,seed);
      acc=hround(acc,group);
      acc=hround(acc,iter);
      acc=hround(acc,pos);
      return hfinal(acc);
    }

    void create_bonds(ILog *log, unsigned n, uint32_t seed, unsigned step,  uint32_t prob, const int *spins, int *up_down, int *left_right) const
    {
      log->LogVerbose("  create_bonds %u", step);
      for(unsigned y=0; y<n; y++){
        for(unsigned x=0; x<n; x++){
          bool sC=spins[y*n+x];
          
          bool sU=spins[ ((y+1)%n)*n + x ];
          if(sC!=sU){
            up_down[y*n+x]=0;
          }else{
            up_down[y*n+x]=hrng(seed, rng_group_bond_ud, step, y*n+x) < prob;
          }

          bool sR=spins[ y*n + (x+1)%n ];
          if(sC!=sR){
            left_right[y*n+x]=0;
          }else{
            left_right[y*n+x]=hrng(seed, rng_group_bond_lr, step, y*n+x) < prob;
          }
        }
      }
    }

    void create_clusters(ILog *log, unsigned n, uint32_t /*seed*/, unsigned step, const int *up_down, const int *left_right, unsigned *cluster) const
    {
      log->LogVerbose("  create_clusters %u", step);

      for(unsigned i=0; i<n*n; i++){
        cluster[i]=i;
      }

      bool finished=false;
      unsigned diameter=0;
      while(!finished){
        diameter++;
        finished=true;
        for(unsigned y=0; y<n; y++){
          for(unsigned x=0; x<n; x++){
            unsigned prev=cluster[y*n+x];
            unsigned curr=prev;
            if(left_right[y*n+x]){
              curr=std::min(curr, cluster[y*n+(x+1)%n]);
            }
            if(left_right[y*n+(x+n-1)%n]){
              curr=std::min(curr, cluster[y*n+(x+n-1)%n]);
            }
            if(up_down[y*n+x]){
              curr=std::min(curr, cluster[ ((y+1)%n)*n+x]);
            }
            if(up_down[((y+n-1)%n)*n+x]){
              curr=std::min(curr, cluster[ ((y+n-1)%n)*n+x]);
            }
            if(curr!=prev){
              cluster[y*n+x]=curr;
              finished=false;
            }
          }
        }
      }
      log->LogVerbose("    diameter %u", diameter);
    }

    void flip_clusters(ILog *log, unsigned n, uint32_t seed, unsigned step, unsigned *clusters, int *spins) const
    {
      log->LogVerbose("  flip_clusters %u", step);

      for(unsigned i=0; i<n*n; i++){
        unsigned cluster=clusters[i];
        if(hrng(seed, rng_group_flip, step, cluster) >> 31){
          spins[i] ^= 1;
        }
      }
    }

    void count_clusters(ILog *log, unsigned n, uint32_t /*seed*/, unsigned step, const unsigned *clusters, unsigned *counts, unsigned &nClusters) const
    {
      log->LogVerbose("  count_clusters %u", step);

      for(unsigned i=0; i<n*n; i++){
        counts[i]=0; 
      }
      for(unsigned i=0; i<n*n; i++){
        counts[clusters[i]]++;
      }
      
      nClusters=0;
      for(unsigned i=0; i<n*n; i++){
        if(counts[i]){
          nClusters++;
        }
      }
    }

    
    void ReferenceExecute(
			  ILog *log,
			  const IsingInput *pInput,
			  IsingOutput *pOutput
			  ) const
    {
      
      log->LogInfo("Building world");
      unsigned n=pInput->n;
      uint32_t prob=pInput->prob;
      uint32_t seed=pInput->seed;
      std::vector<int> spins(n*n);
      std::vector<int> left_right(n*n);
      std::vector<int> up_down(n*n);
      std::vector<unsigned> clusters(n*n);
      std::vector<unsigned> counts(n*n);
      for(unsigned i=0; i<n*n; i++){
        spins[i]=hrng(seed, rng_group_init, 0, i) & 1;
      }

      log->LogInfo("Doing iterations");
      std::vector<uint32_t> stats(n);

      for(unsigned i=0; i<n; i++){
        log->LogVerbose("  Iteration %u", i);
        create_bonds(   log, n, seed, i, prob, &spins[0], &up_down[0], &left_right[0]);
        create_clusters(log,  n, seed, i,                  &up_down[0], &left_right[0], &clusters[0]);
        flip_clusters(  log,  n, seed, i,                                               &clusters[0], &spins[0]);
        count_clusters( log,  n, seed, i,                                               &clusters[0], &counts[0], stats[i]);
        log->LogVerbose("  clusters count is %u", stats[i]);

        log->Log( Log_Debug, [&](std::ostream &dst){
          dst<<"\n";
          for(unsigned y=0; y<n; y++){
            for(unsigned x=0; x<n; x++){
              dst<<(spins[y*n+x]?"+":" ");
            }
            dst<<"\n";
          }
        });
      }
      
      pOutput->history=stats;
      log->LogInfo("Finished");
    }

    virtual void Execute(
      ILog *log,
      const IsingInput *input,
      IsingOutput *output
      ) const {
      log->LogInfo("Using ReferenceExecute to implement Execute. Should this be overriden?");
      ReferenceExecute(log, input, output); 
    }

  public:
    virtual std::string Name() const override
    { return "ising"; }

    virtual bool HasBitExactOutput() const override
    { return true; }

    virtual bool CompareOutputs(ILog * /*log*/, const IsingInput * /*input*/, const IsingOutput *ref, const IsingOutput *got) const override
    {
      return ref->history==got->history;
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
      
      auto params=std::make_shared<IsingInput>(this, scale);

      params->n=scale;
      params->seed=rnd();
      params->prob=ceil(ldexp( udist(rnd)*0.2+0.4, +32 ));

      return params;
    }

  };

};

#endif
