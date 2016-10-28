/*
  Got help from http://www.gamedev.net/page/resources/_/technical/game-programming/bvh-file-loading-and-displaying-r3295
*/
#include "skeleton.h"

using namespace std;
using namespace Eigen;

void writeTabs(ofstream & str, int tabs); // for outputting file


// Get skeleton data from bvh file
void skeleton::readBvhFile(char* fileName)
{
  string line, token;
  ifstream infile(fileName);
  if (!infile){
    cerr << "Error: unable to open input file: " << fileName << endl;
    exit(1);
  }

  getline(infile, line);
  if (line != "HIERARCHY"){
    cerr << "File " << fileName << " doesn't start with HIERARCHY" << endl;
    exit(1);
  }

  // load hierarchy
  while(infile.good())
  {
    infile >> token;
    if (token == "ROOT")
      rootJoint = loadJoint(infile,nullptr, 0);
    else if (token == "MOTION")
      loadMotion(infile);
  }
  
  infile.close();
}

// recursive function to load joints
JOINT* skeleton::loadJoint(istream & str, JOINT* parent, int type)
{
  cout << "Loading joint ";
  JOINT* joint = new JOINT;
  joint->parent = parent;
  
  string* name =  new string;
  str >> *name; // Name is 'Hips' at first call
  joint->name = name->c_str();

  cout << *name << endl;

  joint->matrix = glm::mat4(1.0); // identity matrix

  joint->joint_type = type; // 0 = root, 1 = node, 2 = leaf

  string token;

  static int channel_start_index = 0; // used to determine where each joint's channel is in MOTION channel array
  unsigned int channel_order_index = 0;

  while(str.good())
  {
    str >> token; // if none of the ifs are read, this will be called again (Eg. token == '{', or token == 'Xposition' because I'm ignoring those)

    char c = token.at(0);
    if (c == 'X' || c == 'Y' || c == 'Z')
    {
      if (token == "Xposition")
      {
        joint->channels_order[channel_order_index++] = XPOS;
      }
      if (token == "Yposition")
      {
        joint->channels_order[channel_order_index++] = YPOS;
      }
      if (token == "Zposition")
      {
        joint->channels_order[channel_order_index++] = ZPOS;
      }
      if (token == "Xrotation")
      {
        joint->channels_order[channel_order_index++] = XROT;
      }
      if (token == "Yrotation")
      {
        joint->channels_order[channel_order_index++] = YROT;
      }
      if (token == "Zrotation")
      {
        joint->channels_order[channel_order_index++] = ZROT;
      }
    }
    if (token == "CHANNELS")
    {
      // ignore except number of channels for reading motion part
      str >> joint->num_channels;
      motionData.num_motion_channels += joint->num_channels;

      // need to store channel indices and such to determine which joint relates to which numbers in the motion data
      joint->channel_start = channel_start_index;
      // move index so next joint knows to start num_channels later
      channel_start_index += joint->num_channels;
      joint->channels_order = new int[joint->num_channels];
    }
    else if (token == "OFFSET")
    {
      // read in offset values
      str >> joint->offset[0] >> joint->offset[1] >> joint->offset[2];
    }
    else if (token == "JOINT")
    {
      // recursive call to loadJoint with type 1
      JOINT* tmp_joint = loadJoint(str, joint, 1);
      // add tmp_joint to joint's children
      tmp_joint->parent = joint;
      joint->children.push_back(tmp_joint);
    }
    else if (token == "End")
    {
      // get past 'Site' and '{'
      str >> token >> token;

      JOINT* tmp_joint = new JOINT;
      tmp_joint->parent = joint;

      string* name = new string;
      *name = "EndSite";
      tmp_joint->name = name->c_str();
      tmp_joint->joint_type = 2; // leaf

      // Add tmp_joint to joint's children
      joint->children.push_back(tmp_joint);

      // Check if there's an offset
      str >> token;
      // 
      if (token == "OFFSET"){
        str >> tmp_joint->offset[0] >> tmp_joint->offset[1] >> tmp_joint->offset[2];
      }
      str >> token; // End Site '}'
    }
    else if (token == "}")
    {
      cout << "Finished loading " << joint->name << endl;
      return joint; // Recursion!
    }
  }


}

// load motion data
void skeleton::loadMotion(istream & str)
{
  cout << "Loading motion" << endl;
  string token;

  while(str.good())
  {
    str >> token;
    if (token == "Frames:")
    {
      str >> motionData.num_frames;
    }
    else if (token == "Frame") //Frame Time:
    {
      str >> token; // "Time:"
      str >> motionData.frame_time;

      int num_frames = motionData.num_frames, num_channels = motionData.num_motion_channels;

      // allocate space for motion data array
      motionData.data = new float[num_frames * num_channels];

      // for each frame (line) in file read and store floats
      for(int frame = 0; frame < num_frames; ++frame)
      {
        cout << "Reading frame\n";
        for(int channel = 0; channel < num_channels; ++channel) 
        {
          float x;
          str >> x;

          int ind = frame * num_channels + channel;
          motionData.data[ind] = x;
        }
      }

    }
  }

}


// Write current skeleton to Bvh file
void skeleton::writeBvhFile(char* fileName)
{
  ofstream outfile(fileName);
  if(!outfile){
    cerr << "Error: unable to open output file: " << fileName << endl;
    exit(1);
  }

  outfile << "HIERARCHY\nROOT ";
  writeJoint(outfile, rootJoint, 0); // start of root should have 0 tabs
  outfile << "}\n";

  writeMotion(outfile);

  outfile.close();

}

int skeleton::writeJoint(ofstream & str, JOINT* joint, int tabs){
  cout << "Writing Joint: " << joint->name << endl;
  // write name
  if (joint->joint_type != 2) str << joint->name << endl;
  else str << "End Site\n";

  writeTabs(str,tabs);

  str << "{\n";
  // inner object should be tabbed more
  tabs += 1;
  writeTabs(str,tabs);
  str << "OFFSET "  << joint->offset[0] << " " << joint->offset[1] << " " << joint->offset[2] << endl;

  if (joint->joint_type == 0 || joint->joint_type == 1) // root or node
  {
    writeTabs(str,tabs);
    str << "CHANNELS " << joint->num_channels << " ";
    if (joint->joint_type == 0) str << "Xposition Yposition Zposition Zrotation Xrotation Yrotation\n";
    else str << "Zrotation Yrotation Xrotation\n";

    for (int i = 0; i < joint->children.size(); ++i)
    {
      writeTabs(str,tabs);
      if (joint->children[i]->joint_type != 2) str << "JOINT ";
      tabs = writeJoint(str, joint->children[i], tabs);
      if (tabs < 0) tabs = 0;
      writeTabs(str,tabs);
      str << "}\n";
    }
    return tabs - 1;
  }
  else // leaf
  {
    return tabs - 1;
  }

}

void skeleton::writeMotion(ofstream & str){
  cout << "Writing Motion\n";
  str << "MOTION\n";
  str << "Frames: " << motionData.num_frames << endl;
  str << "Frame Time: " << motionData.frame_time << endl;

  int num_frames = motionData.num_frames, num_channels = motionData.num_motion_channels;

  for(int frame = 0; frame < num_frames; ++frame)
    {
      for(int channel = 0; channel < num_channels; ++channel) 
      {
        int ind = frame * num_channels + channel;
        
        str << motionData.data[ind] << " ";
      }
      str << endl;
    }
}

// write tabs
void writeTabs(ofstream & str, int tabs){
  for (int i = 0; i < tabs; ++i)
  {
    str << '\t';
  }
}

void skeleton::moveJoint (JOINT * joint, MOTION * motionData, unsigned int frame_starts_index)
{
  // index of motion data's array for this joint
  int start_ind = frame_starts_index + joint->channel_start;

  joint->matrix = glm::translate(glm::mat4(1.0),glm::vec3(joint->offset[0],joint->offset[1],joint->offset[2]));

  for (int i = 0; i < joint->num_channels; ++i)
  {
    const int & channel = joint->channels_order[i];

    float value = motionData->data[start_ind + i];

    if (channel & XPOS)
    {
      joint->matrix = glm::translate(joint->matrix, glm::vec3(value,0,0));
    }
    if (channel & YPOS)
    {
      joint->matrix = glm::translate(joint->matrix, glm::vec3(0,value,0));
    }
    if (channel & ZPOS)
    {
      joint->matrix = glm::translate(joint->matrix, glm::vec3(0,0,value));
    }
    if (channel & XROT)
    {
      joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(1,0,0));
    }
    if (channel & YROT)
    {
      joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(0,1,0));
    }
    if (channel & ZROT)
    {
      joint->matrix = glm::rotate(joint->matrix, value, glm::vec3(0,0,1));
    }
  }

  if (joint->parent != NULL){
    joint->matrix = joint->parent->matrix * joint->matrix;
  }

  for (auto & child: joint->children){
    moveJoint(child, motionData, frame_starts_index);
  }
}

void skeleton::moveTo(unsigned int frame)
{
  unsigned int start_index = frame * motionData.num_motion_channels;

  moveJoint(rootJoint, &motionData, start_index);
}

void skeleton::setVertices(JOINT * joint, GLshort parentIndex)
{
  glm::vec4 translatedVertex = joint->matrix[3];

  vertices.push_back(translatedVertex);

  GLshort myIndex = vertices.size() - 1;
  if (parentIndex != myIndex)
  {
    indices.push_back(parentIndex);
    indices.push_back(myIndex);
  }

  for(auto & child : joint->children)
  {
    setVertices(child, myIndex);
  }
}
