#ifndef ScoreManager_H
#define ScoreManager_H

#include "Grade.h"
#include "GameConstantsAndTypes.h"
#include "PrefsManager.h"

#include <map>
#include <unordered_map>
#include <string>

using std::string;


// Scores for a specific rate for a specific chart
struct ScoresAtRate
{
public:
	ScoresAtRate();

	HighScore* PBptr;

	// -technically- your pb could be a fail grade so use "bestgrade" -mina
	Grade bestGrade;

	void AddScore(HighScore& hs);

	vector<string> GetSortedKeys();
	void PushSelf(lua_State *L);
	

	XNode* CreateNode(const int& rate) const;
	void LoadFromNode(const XNode* node, const string& key, const float& rate);

	const vector<HighScore*> GetScores(const string) const;
	unordered_map<string, HighScore> scores;
private:

};


// All scores for a specific chart
struct ScoresForChart
{
public:
	ScoresForChart();


	Grade bestGrade = Grade_Invalid;	// best grade for any rate 

	HighScore* GetPBAt(float& rate);
	HighScore* GetPBUpTo(float& rate);

	vector<HighScore*> GetAllPBPtrs();

	void AddScore(HighScore& hs);
	void AddScore(HighScore& hs, const string& title);

	vector<float> GetPlayedRates();
	vector<int> GetPlayedRateKeys();
	vector<string> GetPlayedRateDisplayStrings();
	string RateKeyToDisplayString(float rate);
	int RateToKey(float& rate) { return lround(rate * 10000.f); }
	float KeyToRate(int key) { return static_cast<float>(key) / 10000.f; }

	void PushSelf(lua_State *L);

	RString LastSeenPack;
	RString LastSeenSong;
	

	ScoresAtRate* GetScoresAtRate(const int& rate);
	XNode *CreateNode(const string& ck) const;
	void LoadFromNode(const XNode* node, const string& ck);

	ScoresAtRate operator[](const int rate) { return ScoresByRate.at(rate); }
	map<int, ScoresAtRate, greater<int>> ScoresByRate;
private:
	/* It makes sense internally to have the map keys sorted highest rate to lowest
	however my experience in lua is that it tends to be more friendly to approach things
	in the reverse -mina */
	
};

class ScoreManager
{
public:
	ScoreManager();
	~ScoreManager();


	HighScore* GetChartPBAt(string& ck, float& rate);

	// technically "up to and including rate: x" but that's a mouthful -mina
	HighScore* GetChartPBUpTo(string& ck, float& rate);

	Grade GetBestGradeFor(string& ck) { if (pscores.count(ck)) return pscores[ck].bestGrade; return Grade_Invalid; }

	// for scores achieved during this session
	void AddScore(const HighScore& hs_) { HighScore hs = hs_; pscores[hs.GetChartKey()].AddScore(hs); }

	// temporarily to be used for conversion of old ett.xml and not only adding scores obtained in-session
	void AddScore(const HighScore& hs_, const string& ck, const float& rate, const string& title);
	float minpercent = PREFSMAN->m_fMinPercentToSaveScores;	// until i can get this shit to load properly the right way -mina

	// Player Rating and SSR functions
	void SortTopSSRPtrs(Skillset ss);
	void RecalculateSSRs();
	void EnableAllScores();
	void CalcPlayerRating(float& prating, float* pskillsets);
	float AggregateSSRs(Skillset ss, float rating, float res, int iter) const;

	float GetTopSSRValue(unsigned int rank, int ss);

	HighScore * GetTopSSRHighScore(unsigned int rank, int ss);
	
	bool KeyHasScores(const string& ck) { return pscores.count(ck) == 1; }
	bool HasAnyScores() { return !AllScores.empty(); }

	

	XNode *CreateNode() const;
	void LoadFromNode(const XNode* node);

	ScoresForChart* GetScoresForChart(const string& ck);
	vector<string> GetSortedKeys();


	void PushSelf(lua_State *L);


	vector<HighScore*> GetAllScores() { return AllScores; }
	void RegisterScore(HighScore* hs) {	AllScores.emplace_back(hs); }
	void AddToKeyedIndex(HighScore* hs) { ScoresByKey.emplace(hs->GetScoreKey(), hs); }
private:
	unordered_map<string, ScoresForChart> pscores;	// Profile scores

	// Instead of storing pointers for each skillset just reshuffle the same set of pointers
	// it's inexpensive and not called often
	vector<HighScore*> TopSSRs;
	vector<HighScore*> AllScores;

	// pointers in a keyed index
	unordered_map<string, HighScore*> ScoresByKey;
};

extern ScoreManager* SCOREMAN;

#endif