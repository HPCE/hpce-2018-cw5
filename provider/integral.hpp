#ifndef user_integral_hpp
#define user_integral_hpp

#include "puzzler/puzzles/integral.hpp"


class IntegralProvider
  : public puzzler::IntegralPuzzle
{
public:
  IntegralProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::IntegralInput *input,
				puzzler::IntegralOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
