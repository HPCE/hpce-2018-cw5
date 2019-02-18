#ifndef user_decompose_hpp
#define user_decompose_hpp

#include "puzzler/puzzles/decompose.hpp"


class DecomposeProvider
  : public puzzler::DecomposePuzzle
{
public:
  DecomposeProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::DecomposeInput *input,
				puzzler::DecomposeOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
