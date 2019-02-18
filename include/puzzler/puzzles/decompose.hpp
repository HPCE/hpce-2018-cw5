#ifndef puzzler_puzzles_decompose_hpp
#define puzzler_puzzles_decompose_hpp

#include <random>
#include <sstream>
#include <vector>
#include <climits>
#include <cassert>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{

  class DecomposePuzzle;
  class DecomposeInput;
  class DecomposeOutput;

  class DecomposeInput
    : public Puzzle::Input
  {
  public:
    uint32_t n;
    uint32_t seed;
    
    DecomposeInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    DecomposeInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override final
    {
      conn.SendOrRecv(n);
      conn.SendOrRecv(seed);
    }
  };

  class DecomposeOutput
    : public Puzzle::Output
  {
  public:
    uint64_t hash;

    DecomposeOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    DecomposeOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override
    {
      conn.SendOrRecv(hash);
    }

  };

  class DecomposePuzzle
    : public PuzzleBase<DecomposeInput,DecomposeOutput>
  {
  protected:
    const unsigned P=7;

    uint32_t make_bit(uint32_t seed, uint32_t input) const
    {
      const uint32_t PRIME32_1  = 2654435761U;
      const uint32_t PRIME32_2 = 2246822519U;
      seed += input * PRIME32_2;
      seed  = (seed<<13) | (seed>>(32-13));
      seed *= PRIME32_1;
      return seed % P;
    }

    unsigned mul(unsigned a, unsigned b) const
    { return (a*b)%P; }

    unsigned sub(unsigned a, unsigned b) const
    { return (P+a-b)%P; }

    unsigned mul_inv(unsigned x) const
    {
      assert(x!=0);
      for(unsigned i=1; i<P; i++){
        if( ((i*x)%P) == 1 ){
          return i;
        }
      }
      assert(0);
      return 0;
    }

    unsigned div(unsigned a, unsigned b) const
    { return mul(a, mul_inv(b) ); }

    void dump(ILog *log, int level, unsigned rr, unsigned cc, uint32_t *matrix) const
    {
      if(level <= log->Level() ){
        for(unsigned r=0; r<rr; r++){
          std::stringstream acc;
          for(unsigned c=0; c<cc; c++){
            acc<<matrix[rr*c+r];
          }
          log->Log(level, acc.str().c_str());
        }
      }
    }

    void decompose(ILog *log, unsigned rr, unsigned cc, unsigned p, uint32_t *matrix) const
    {
      auto at = [=](unsigned r, unsigned c) -> uint32_t &{
        assert(r<rr && c<cc);
        return matrix[rr*c+r];
      };

      dump(log, Log_Debug, rr, cc, matrix);

      unsigned rank=0;
      for(unsigned c1=0; c1<cc; c1++){
        unsigned r1=rank;
        while(r1<rr && at(r1,c1)==0){
          ++r1;
        }

        if(r1!=rr){
          unsigned pivot=at(r1,c1);
          for(unsigned c2=0; c2<cc; c2++){
            std::swap( at(r1,c2), at(rank,c2) );
            at(rank,c2)=div( at(rank,c2) , pivot );
          }

          for(unsigned r2=rank+1; r2<rr; r2++){
            unsigned count=at(r2, c1);
            for(unsigned c2=0; c2<cc; c2++){
              at(r2,c2) = sub( at(r2,c2) , mul( count, at(rank,c2)) );
            }
          }

          ++rank;
        }

        dump(log, Log_Debug, rr, cc, matrix);
      }
    }

    void ReferenceExecute(
			  ILog *log,
			  const DecomposeInput *pInput,
			  DecomposeOutput *pOutput
			  ) const
    {
      unsigned n=pInput->n;
      unsigned rr=n;
      unsigned cc=n;
      unsigned p=7;
      
      log->LogInfo("Building random matrix");
      std::vector<uint32_t> matrix(rr*cc);
      for(unsigned i=0; i<matrix.size(); i++){
        matrix[i]=make_bit(pInput->seed, i);
      }
      dump(log, Log_Verbose, rr, cc, &matrix[0]);
      
      log->LogInfo("Doing the decomposition");
      decompose(log, rr, cc, p, &matrix[0]);
      
      log->LogInfo("Collecting decomposed hash.");
      dump(log, Log_Verbose, rr, cc, &matrix[0]);
      uint64_t hash=0;
      for(unsigned i=0; i<matrix.size(); i++){
        hash += uint64_t(matrix[i])*i;
      }
      pOutput->hash=hash;
      
      log->LogInfo("Finished");
    }

    virtual void Execute(
      ILog *log,
      const DecomposeInput *input,
      DecomposeOutput *output
      ) const
    {
      log->LogInfo("Using ReferenceExecute to implement Execute. Should this be overriden?");
      ReferenceExecute(log, input, output); 
    }

  public:
    virtual std::string Name() const override
    { return "decompose"; }

    virtual bool HasBitExactOutput() const override
    { return true; }

    virtual bool CompareOutputs(ILog * /*log*/, const DecomposeInput * /*input*/, const DecomposeOutput *ref, const DecomposeOutput *got) const override
    {
      return ref->hash==got->hash;
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
      
      auto params=std::make_shared<DecomposeInput>(this, scale);

      params->n=scale;
      params->seed=rnd();

      return params;
    }

  };

};

#endif
