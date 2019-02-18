#ifndef puzzler_puzzles_Integral_hpp
#define puzzler_puzzles_Integral_hpp

#include <random>
#include <sstream>
#include <vector>
#include <climits>
#include <cassert>

#include "puzzler/core/puzzle.hpp"

namespace puzzler
{

  class IntegralPuzzle;
  class IntegralInput;
  class IntegralOutput;

  class IntegralInput
    : public Puzzle::Input
  {
  public:
    int resolution;
    std::vector<float> C; 
    std::vector<float> M;
    std::vector<float> bounds;
    
    IntegralInput(const Puzzle *puzzle, int scale)
      : Puzzle::Input(puzzle, scale)
    {}

    IntegralInput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Input(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override final
    {
      conn.SendOrRecv(resolution);
      conn.SendOrRecv(C);
      conn.SendOrRecv(M);
      conn.SendOrRecv(bounds);
    }
  };

  class IntegralOutput
    : public Puzzle::Output
  {
  public:
    double value;

    IntegralOutput(const Puzzle *puzzle, const Puzzle::Input *input)
      : Puzzle::Output(puzzle, input)
    {}

    IntegralOutput(std::string format, std::string name, PersistContext &ctxt)
      : Puzzle::Output(format, name, ctxt)
    {
      PersistImpl(ctxt);
    }

    virtual void PersistImpl(PersistContext &conn) override
    {
      conn.SendOrRecv(value);
    }

  };

  class IntegralPuzzle
    : public PuzzleBase<IntegralInput,IntegralOutput>
  {
  public:
    static const int D=3;  
  protected:
    /* Note: to keep things simple this doesn't normalise weighting according to the linear
      transform, so it no longer sums to 1. However, we'll ignore that.
    */

    float updf(float x) const
    {
      return exp(-x*x/2) / sqrt(2*3.1415926535897932384626433832795);
    }

    float mpdf(int r, float range, float x[D], const float M[D*D], const float C[D], const float bounds[D]) const
    {
      float dx=range/r;

      float acc=1.0f;
      for(unsigned i=0; i<D; i++){
        float xt=C[i];
        for(unsigned j=0; j<D; j++){
          xt += M[i*D+j] * x[j];
        }
        acc *= updf(xt) * dx;
      }

      for(unsigned i=0; i<D;i++){
        if(x[i] > bounds[i]){
          acc=0;
        }
      }

      return acc;
    }

    void ReferenceExecute(
			  ILog *log,
			  const IntegralInput *pInput,
			  IntegralOutput *pOutput
			  ) const
    {
      unsigned r=pInput->resolution;

      const float range=12;

      double acc=0;
      for(unsigned i1=0; i1<r; i1++){
        for(unsigned i2=0; i2<r; i2++){
          for(unsigned i3=0; i3<r; i3++){
            float x1= -range/2 + range * (i1/(float)r);
            float x2= -range/2 + range * (i2/(float)r);
            float x3= -range/2 + range * (i3/(float)r);

            float x[3]={x1,x2,x3};
            acc += mpdf(r, range, x, &pInput->M[0], &pInput->C[0], &pInput->bounds[0]);
          }
        }
      }

      log->LogInfo("Integral = %g", acc);
      pOutput->value=acc;
    }

    virtual void Execute(
      ILog *log,
      const IntegralInput *input,
      IntegralOutput *output
      ) const 
    {
      log->LogInfo("Using ReferenceExecute to implement Execute. Should this be overriden?");
      ReferenceExecute(log, input, output); 
    }

  public:
    virtual std::string Name() const override
    { return "integral"; }

    virtual bool HasBitExactOutput() const override
    { return false; }

    virtual bool CompareOutputs(ILog * /*log*/, const IntegralInput * input, const IntegralOutput *ref, const IntegralOutput *got) const override
    {
      if(ref->value==0){
        throw std::runtime_error("This is a bug, as ref->value should always be positive.");
      }
      return std::abs( (ref->value - got->value) / ref->value ) < pow(input->resolution,1.5) * 1e-8;
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

      const int D=IntegralPuzzle::D;
      
      auto params=std::make_shared<IntegralInput>(this, scale);

      params->bounds.resize(D);
      params->M.resize(D*D, 0);
      params->C.resize(D);

      params->resolution=20+scale;
      for(unsigned i=0; i<D; i++){
        params->bounds[i]=udist(rnd)*3-1.5;
        params->C[i]=(udist(rnd)-0.5)*0.5;
        params->M[i*D+i]=1;
        for(unsigned j=0;j<D;j++){
          params->M[i*D+j] += (udist(rnd)-0.5)*0.5;
        }
      }

      return params;
    }

  };

};

#endif
