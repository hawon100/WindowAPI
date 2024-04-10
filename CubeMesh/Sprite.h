#pragma once
class Sprite
{
public:
	Sprite();
	Sprite(int w, int h);
	Sprite(std::wstring sFile);

	int nWidth = 0;
	int nHeight = 0;

private:
	short* m_Glyphs = nullptr;
	short* m_Colours = nullptr;

	virtual void Create(int w, int h);

public:
	virtual void SetGlyph(int x, int y, short c);
	virtual void SetColour(int x, int y, short c);

	virtual short GetGlyph(int x, int y);
	virtual short GetColour(int x, int y);

	virtual short SampleGlyph(float x, float y);
	virtual short SampleColour(float x, float y);

	virtual bool Save(std::wstring sFile);
	virtual bool Load(std::wstring sFile);
};