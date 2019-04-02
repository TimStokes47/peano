#ifndef _VISUALISATION_INPUT_PEANO_PATCH_FILE_READER_H_
#define _VISUALISATION_INPUT_PEANO_PATCH_FILE_READER_H_


class PeanoPatch;


namespace visualisation {
  namespace input {
    class PatchFileReader;
  }
}



class visualisation::input::PatchFileReader {
  public:
	virtual void parse() = 0;
	virtual bool isEmpty() = 0;
	virtual std::vector<PeanoPatch*> getPatches() = 0;
};

#endif
