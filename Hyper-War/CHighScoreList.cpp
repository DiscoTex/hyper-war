/*************************************************************/
/*	CEffects.h: Class for rendering global effects			 */
/*															 */
/*	July 2009												 */
/*************************************************************/
#include "CHighScoreList.h"

CHighScoreList::CHighScoreList(GLFont* newScoreFont)
{
	hsFont = newScoreFont;
}

CHighScoreList::~CHighScoreList()
{
}

void CHighScoreList::ReadScores()
{
	FILE* scoreFile;
	HSListEntry entry;
	errno_t err = 0;
	char tempBuf[16];
	char *res = NULL;

	if(fopen_s(&scoreFile, "highscores.txt", "r") == 0)
	{
		for(int i=0; i<10 && scoreFile; i++)
		{
			res = fgets(entry.name, 16, scoreFile);
			entry.name[strnlen(entry.name, 16)-1] = '\0';
			res = fgets(tempBuf, 16, scoreFile);
			sscanf_s(tempBuf, "%d", &entry.score);
			if(res != NULL)
				HSList.push_back(entry);
		}
		fclose(scoreFile);
	}

	std::sort(HSList.begin(), HSList.end(), SortFunc);
}

void CHighScoreList::SaveScores()
{
	FILE* scoreFile;
	HSListEntry entry;
	errno_t err = 0;

	if(fopen_s(&scoreFile, "highscores.txt", "w+") == 0)
	{
		for(int i=0; i<10 && err != -1; i++)
		{
			err = fprintf_s(scoreFile, "%s\n%d\n", HSList[i].name, HSList[i].score);
			if(err != -1)
				HSList.push_back(entry);
		}
		fclose(scoreFile);
	}
}

bool CHighScoreList::AddScore(char* name, int score)
{
	HSListEntry entry;
	addIndex = -1;

	for(unsigned int i=0; i<10 && addIndex == -1; i++)
	{
		if(score > HSList[i].score)
			addIndex = i;
	}

	if(addIndex != -1)
	{
		strncpy_s(entry.name, 16, name, 16);
		entry.score = score;
		HSList.push_back(entry);
	}

	std::sort(HSList.begin(), HSList.end(), SortFunc);

	return (addIndex != -1);
}

void CHighScoreList::Draw(bool highlightRecent)
{
	int height, width;
	unsigned int i;
	char tempStr[64];

	glPushMatrix();

	glTranslatef(0, 1.5, 0);

	glPushMatrix();
	width = 0;
	height = 0;
	sprintf_s(tempStr, 64, "HIGH SCORES");
	for(i=0; i<strnlen(tempStr, 64); i++)
	{
		width += hsFont->GetCharWidthA(tempStr[i]);
	}
	height = hsFont->GetCharHeight('D');
	glScalef(.006f, .006f, .006f);
	hsFont->Begin();
	glTranslatef(-width/2.0f, 0, 0);	
	hsFont->DrawString(tempStr, 0, 0);
	glPopMatrix();

	for(i=0; i<10; i++)
	{
		glPushMatrix();
		width = 0;
		height = 0;
		sprintf(tempStr, "%s   %d", HSList[i].name, HSList[i].score);
		for(unsigned int j=0; j<strnlen(tempStr, 64); j++)
		{
			width += hsFont->GetCharWidthA(tempStr[j]);
		}
		height = hsFont->GetCharHeight('D');
		glScalef(.0026f, .0026f, .0026f);
		hsFont->Begin();
		glTranslatef(-width/2.0f, -100.0f * (i + 2), 0);	
		hsFont->DrawString(tempStr, 0, 0);
		glPopMatrix();
	}

	glPopMatrix();
}

bool SortFunc(HSListEntry first, HSListEntry last)
{
	return first.score > last.score;
}