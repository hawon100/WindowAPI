#include "ConsoleGameEngine.h"
#include <string>

class PanAndZoom : public ConsoleGameEngine
{
public:
	PanAndZoom()
	{
		m_sAppName = L"Zooming";
	}

private:

	float fOffsetX = 0.0f;
	float fOffsetY = 0.0f;
	float fScaleX = 1.0f;
	float fScaleY = 1.0f;

	float fStartPanX = 0.0f;
	float fStartPanY = 0.0f;

	float fSelectedCellX = 0.0f;
	float fSelectedCellY = 0.0f;


protected:

	virtual bool OnUserCreate()
	{		
		fOffsetX = -ScreenWidth() / 2;
		fOffsetY = -ScreenHeight() / 2;
		return true;
	}

	void WorldToScreen(float fWorldX, float fWorldY, int &nScreenX, int &nScreenY)
	{
		nScreenX = (int)((fWorldX - fOffsetX) * fScaleX);
		nScreenY = (int)((fWorldY - fOffsetY) * fScaleY);
	}

	void ScreenToWorld(int nScreenX, int nScreenY, float &fWorldX, float &fWorldY)
	{
		fWorldX = ((float)nScreenX / fScaleX) + fOffsetX;
		fWorldY = ((float)nScreenY / fScaleY) + fOffsetY;
	}

	virtual bool OnUserUpdate(float fElapsedTime)
	{
		float fMouseX = (float)GetMouseX();
		float fMouseY = (float)GetMouseY();

		if (GetMouse(2).bPressed)
		{
			fStartPanX = fMouseX;
			fStartPanY = fMouseY;
		}

		if (GetMouse(2).bHeld)
		{
			fOffsetX -= (fMouseX - fStartPanX) / fScaleX;
			fOffsetY -= (fMouseY - fStartPanY) / fScaleY;

			fStartPanX = fMouseX;
			fStartPanY = fMouseY;
		}

		float fMouseWorldX_BeforeZoom, fMouseWorldY_BeforeZoom;
		ScreenToWorld(fMouseX, fMouseY, fMouseWorldX_BeforeZoom, fMouseWorldY_BeforeZoom);

		if (GetKey(L'Q').bHeld)
		{
			fScaleX *= 1.001f;
			fScaleY *= 1.001f;
		}

		if (GetKey(L'A').bHeld)
		{
			fScaleX *= 0.999f;
			fScaleY *= 0.999f;
		}

		float fMouseWorldX_AfterZoom, fMouseWorldY_AfterZoom;
		ScreenToWorld(fMouseX, fMouseY, fMouseWorldX_AfterZoom, fMouseWorldY_AfterZoom);		
		fOffsetX += (fMouseWorldX_BeforeZoom - fMouseWorldX_AfterZoom);
		fOffsetY += (fMouseWorldY_BeforeZoom - fMouseWorldY_AfterZoom);

		Fill(0, 0, ScreenWidth(), ScreenHeight(), PIXEL_SOLID, FG_BLACK);

		float fWorldLeft, fWorldTop, fWorldRight, fWorldBottom;
		ScreenToWorld(0, 0, fWorldLeft, fWorldTop);
		ScreenToWorld(ScreenWidth(), ScreenHeight(), fWorldRight, fWorldBottom);

		auto function = [](float x)
		{
			return sinf(x);
		};


		int nLinesDrawn = 0;
		for (float y = 0.0f; y <= 10.0f; y++)
		{
			if (y >= fWorldTop && y <= fWorldBottom)
			{
				float sx = 0.0f, sy = y;
				float ex = 10.0f, ey = y;

				int pixel_sx, pixel_sy, pixel_ex, pixel_ey;

				WorldToScreen(sx, sy, pixel_sx, pixel_sy);
				WorldToScreen(ex, ey, pixel_ex, pixel_ey);

				DrawLine(pixel_sx, pixel_sy, pixel_ex, pixel_ey, PIXEL_SOLID, FG_WHITE);
				nLinesDrawn++;
			}
		}

		for (float x = 0.0f; x <= 10.0f; x++)
		{
			if (x >= fWorldLeft && x <= fWorldRight)
			{
				float sx = x, sy = 0.0f;
				float ex = x, ey = 10.0f;

				int pixel_sx, pixel_sy, pixel_ex, pixel_ey;

				WorldToScreen(sx, sy, pixel_sx, pixel_sy);
				WorldToScreen(ex, ey, pixel_ex, pixel_ey);

				DrawLine(pixel_sx, pixel_sy, pixel_ex, pixel_ey, PIXEL_SOLID, FG_WHITE);
				nLinesDrawn++;
			}
		}

		if (GetMouse(1).bReleased)
		{
			fSelectedCellX = (int)fMouseWorldX_AfterZoom;
			fSelectedCellY = (int)fMouseWorldY_AfterZoom;
		}

		int cx, cy, cr;
		WorldToScreen(fSelectedCellX + 0.5f, fSelectedCellY + 0.5f, cx, cy);
		cr = 0.3f * fScaleX;
		FillCircle(cx, cy, cr, PIXEL_SOLID, FG_RED);
		DrawString(2, 2, L"Lines Drawn: " + to_wstring(nLinesDrawn));


		float fWorldPerScreenWidthPixel = (fWorldRight - fWorldLeft) / ScreenWidth();
		float fWorldPerScreenHeightPixel = (fWorldBottom - fWorldTop) / ScreenHeight();
		int px, py, opx = 0, opy = 0;
		WorldToScreen(fWorldLeft-fWorldPerScreenWidthPixel, -function((fWorldLeft - fWorldPerScreenWidthPixel) - 5.0f) + 5.0f, opx, opy);
		for (float x = fWorldLeft; x < fWorldRight; x+=fWorldPerScreenWidthPixel)
		{
			float y = -function(x - 5.0f) + 5.0f;
			WorldToScreen(x, y, px, py);
			DrawLine(opx, opy, px, py, PIXEL_SOLID, FG_GREEN);
			opx = px;
			opy = py;
		}
		
		return true;
	}
};


int main()
{
	PanAndZoom demo;
	demo.ConstructConsole(120, 60, 8, 8);
	demo.Start();
	return 0;
}