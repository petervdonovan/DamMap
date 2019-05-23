This is the C++ file for methods of the History class.
An object of this class is used to gather information from other objects that
have done all of the image processing and stored the the lines found in the 
image in the simplest form possible. Updating this object causes it to 
interpret this information into a record of the paths of lines across the 
screen, which is interpreted to map the path of the ROV.

The method update() must be passed a 2-dimensional vector of lines, such that 
those lines are grouped based on how close they are to each other and whether 
they point in the same direction. This vector is created by an object of the 
class LineGroupList.

The method showCurrent() is for debugging, to show the locations of the lines 
in a new window.

The method show() generates a map of the ROV's motion through the black grid 
and shows it in a new window.
