
The specification for the different data text files are as follows:

---------------------------------------------------------------------------------------------------------
config.txt Specification:
---------------------------------------------------------------------------------------------------------

Window:
Window W H FPS
	Width				W			int
	Height				H			int
	FPS Limit			FPS			int

---------------------------------------------------------------------------------------------------------
assets.txt Specification:
---------------------------------------------------------------------------------------------------------

Tilesheet:
Tilesheet N P TN...
	Name				N           std::string (no spaces)
	File Path			P			std::string (no spaces)
	Texture Names...	T			std::string (list of names of each texture it holds
												 no spaces in the texture name itself)

Texture:
Texture N P
	Name				N			std::string (no spaces)
	File Path			P			std::string (no spaces)

Animation:
Animation N T F I
	Name				N			std::string (no spaces)
	Texture Name		T			std::string (no spaces)
	Frame Count			F			int
	Frame Interval		I			int

Font:
Font N P
	Name				N			std::string (no spaces)
	File Path			P			std::string (no spaces)

---------------------------------------------------------------------------------------------------------
Each scene to a playable map or the map editor has its own .txt file.
All these scene's .txt has the same specification.

Map Scene Specification:
---------------------------------------------------------------------------------------------------------

Tile:
Tile N GX GY
	Animation Name		N			std::string (no spaces)
	Grid X Position		GX			float
	Grid Y Position		GY			float

Decoration:
Decoration N X Y
	Animation Name		N			std::string (no spaces)
	X Position			X			float
	Y Position			Y			float

Controlled Character:
Player X Y BW BH S H
	Starting Spawn Pos	X Y			int, int
	Bounding Box Size	BW BH		int, int
	Speed				S			float
	Max Health			H			int