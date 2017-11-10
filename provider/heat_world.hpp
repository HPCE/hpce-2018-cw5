#ifndef user_heat_world_hpp
#define user_heat_world_hpp

#include "puzzler/puzzles/heat_world.hpp"


class HeatWorldProvider
  : public puzzler::HeatWorldPuzzle
{
public:
  HeatWorldProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::HeatWorldInput *input,
					puzzler::HeatWorldOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
