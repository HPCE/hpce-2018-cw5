#ifndef user_edit_distance_hpp
#define user_edit_distance_hpp

#include "puzzler/puzzles/edit_distance.hpp"


class EditDistanceProvider
  : public puzzler::EditDistancePuzzle
{
public:
  EditDistanceProvider()
  {}

	virtual void Execute(
			   puzzler::ILog *log,
			   const puzzler::EditDistanceInput *input,
				puzzler::EditDistanceOutput *output
			   ) const override
	{
		ReferenceExecute(log, input, output);
	}

};

#endif
