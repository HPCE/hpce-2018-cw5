#ifndef user_random_projection_hpp
#define user_random_projection_hpp

#include "puzzler/puzzles/random_projection.hpp"


class RandomProjectionProvider
  : public puzzler::RandomProjectionPuzzle
{
public:
  RandomProjectionProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::RandomProjectionInput *input,
				puzzler::RandomProjectionOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
