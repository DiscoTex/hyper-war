/*************************************************************/
/*	CHighScoreList.h: Class for managing and drawing high    */
/*	score list.												 */
/*															 */
/*	September 2009											 */
/*************************************************************/
#include <windows.h>
#include "glBase.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include <algorithm>

using namespace std;
using namespace glfont;

typedef struct
{
	char name[16];
	int score;
} HSListEntry;

class CHighScoreList
{
public:
	CHighScoreList(GLFont* newScoreFont);
	~CHighScoreList();

	void ReadScores();
	void SaveScores();
	bool AddScore(char* name, int score);

	void Draw(bool highlightRecent);

private:
	vector<HSListEntry> HSList;
	GLFont*				hsFont;
	unsigned int		addIndex;
};

bool SortFunc(HSListEntry first, HSListEntry last);
