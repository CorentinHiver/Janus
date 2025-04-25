#ifndef DATAREADER_5052
#define DATAREADER_5052

#include "LibCo/libCo.hpp"
#include "LibCo/lib.hpp"
#include "TreeSiPM.hpp"

/**
 * @brief In developpement. The idea is to get a file m_handler that can be used as reference for every other object.
 * 
 */
class FileHandler 
{
public:

  enum Extension {
    RAWFILE,
    BINFILE,
    TEXTFILE,
    CSVFILE,
    INVALID
  };

  FileHandler(){}

  FileHandler(const std::string& filename) : m_filename(filename)
  {
    readExtension();
  }

  void readExtension(std::string const & filename = "")
  {
    if (filename != "") m_filename = filename;
    m_extension = getFileExtension(m_filename); // In the filename, extracts after the last . (e.g. test.t.txt -> txt)
    std::transform(m_extension.begin(), m_extension.end(), m_extension.begin(), ::tolower); // Turns the extension to lower case
    auto it = extToType.find(m_extension); // read the extension lookup table (lut)
    if (it != extToType.end()) m_fileType = it->second; // If found in the lut, register the file type
    else m_fileType = INVALID; // If not found in the lut, the file has an invalid extension
  }

  FileHandler::Extension const getFileType() const {return static_cast<FileHandler::Extension>(m_fileType);}

  std::string const & getHeader() const {return m_header;}

private:
  std::string m_filename;
  std::string m_extension;
  int m_fileType = FileHandler::INVALID;
  std::string m_header;

  static std::array<std::string, 4> exts; 
  static std::unordered_map<std::string, int> extToType;

  std::string getFileExtension(std::string const & filename) const
  {
    size_t dotPos = filename.rfind('.'); // Get the position of the last dot in the name
    if (dotPos != std::string::npos) return filename.substr(dotPos + 1); // If there is a dot in the string, return the right part 
    else return ""; // If no dot found return an empty string
  }
};
std::array<std::string, 4> FileHandler::exts = {"frd", "dat", "txt", "csv"}; // The handled extensions
std::unordered_map<std::string, int> FileHandler::extToType = { // Lookup table linking the extension and the enum reference
  {exts[RAWFILE], RAWFILE},   // "frd"
  {exts[BINFILE], BINFILE},   // "dat"
  {exts[TEXTFILE], TEXTFILE}, // "txt"
  {exts[CSVFILE], CSVFILE}    // "csv"
};


class MyDataReader
{
public:
  MyDataReader(std::string filename, bool verbose = false);

  void Open();
  void toRoot();
  
  void rawToRoot(){print("MyDatReader::rawToRoot() : Not reading this kind of file (yet ?)");}
  void binToRoot();
  void textToRoot(){print("MyDatReader::textToRoot() : Not reading this kind of file (yet ?)");}
  void csvToRoot(){print("MyDatReader::csvToRoot() : Not reading this kind of file (yet ?)");}

private:
  bool m_verbose = false;
  FileHandler m_handler;
  TreeSiPM<64> m_tree;
  size_t m_fileType = -1;
  std::string m_filename;
};

MyDataReader::MyDataReader(std::string filename, bool verbose):
m_verbose(verbose), m_filename (filename)
{
  Open();
}

void MyDataReader::toRoot()
{
  switch(m_handler.getFileType())
  {
    case FileHandler::BINFILE: binToRoot(); return;
    case FileHandler::RAWFILE: rawToRoot(); return;
    case FileHandler::TEXTFILE: textToRoot(); return;
    case FileHandler::CSVFILE: csvToRoot(); return;
    case FileHandler::INVALID: default: print("invalid file"); return;
  }
}

void MyDataReader::Open()
{
  m_handler.readExtension(m_filename);
  print( "File type :", int(m_handler.getFileType()));
}

void MyDataReader::binToRoot()
{
  MyBinaryData reader(0, 0); // Force ns = 0, mode = 
  std::ifstream datafile(m_filename,std::ios::binary | std::ios::in);
  
  reader.ReadHeaderBinfile (datafile);
  reader.ComputeBinfileSize(datafile);
  
  m_tree.init();

  auto totsize   = reader.GetEventsSize ();
  auto read_size = reader.GetEventsBegin();

  while (!datafile.eof())
  {
    reader.ReadTmpEvt(datafile);
    m_tree.fill      (reader  );
  }

  m_tree->GetEntries();
  std::cout << m_tree << std::endl;

  m_tree.write("test.root","RECREATE");
    

}

#endif //DATAREADER_5052
