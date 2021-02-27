/*
 * This file is pretty much a direct copy of the code presented in
 *
 * https://casual-effects.com/research/McGuire2021PixelArt/
 * 
 */

#include "graphics/scaler/intern.h"

template<int format>
inline static uint16 luma(uint16 color) {
	if (format == 565) {
		return ((color >> 0) & 0x1F) + ((color >> 5) & 0x3F) + ((color >> 11) & 0x1F) + 1;
	} else {
		return ((color >> 0) & 0x1F) + ((color >> 5) & 0x1F) + ((color >> 10) & 0x1F) + 1;

	}
}

inline static bool all_eq2(uint16 B, uint16 A0, uint16 A1) {
	return ((B ^ A0) | (B ^ A1)) == 0;
}

inline static bool all_eq3(uint16 B, uint16 A0, uint16 A1, uint16 A2) {
	return ((B ^ A0) | (B ^ A1) | (B ^ A2)) == 0;
}

inline static bool all_eq4(uint16 B, uint16 A0, uint16 A1, uint16 A2, uint16 A3) {
	return ((B ^ A0) | (B ^ A1) | (B ^ A2) | (B ^ A3)) == 0;
}

inline static bool any_eq3(uint16 B, uint16 A0, uint16 A1, uint16 A2) {
	return B == A0 || B == A1 || B == A2;
}

inline static bool none_eq2(uint16 B, uint16 A0, uint16 A1) {
	return (B != A0) && (B != A1);
}

inline static bool none_eq4(uint16 B, uint16 A0, uint16 A1, uint16 A2, uint16 A3) {
	return B != A0 && B != A1 && B != A2 && B != A3;
}

inline static int clamp(int x, int a, int b)
{
	return (x < a) ? a : (x < b ? x : b);
}

template<int bitFormat>
static void MMPXTemplate(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch, int width, int height) {
	auto src = [=](int x, int y) {
		x = clamp(x, 0, width);
		y = clamp(y, 0, height);

		auto *bP = (const uint16 *)(srcPtr + (y * srcPitch + x * 2));

		return *bP;
	};

	auto dst = [=](int x, int y, uint16 color) {
		assert(x >= 0 && x < width * 2);
		assert(y >= 0 && y < height * 2);

		auto *dP = (uint16 *)(dstPtr + (y * dstPitch + x * 2));
		*dP = color;
	};

	for (int srcY = 0; srcY < height; ++srcY) {
		int srcX = 0;

		// Inputs carried along rows
		uint16 A = src(srcX - 1, srcY - 1);
		uint16 B = src(srcX, srcY - 1);
		uint16 C = src(srcX + 1, srcY - 1);

		uint16 D = src(srcX - 1, srcY);
		uint16 E = src(srcX, srcY);
		uint16 F = src(srcX + 1, srcY);

		uint16 G = src(srcX - 1, srcY + 1);
		uint16 H = src(srcX, srcY + 1);
		uint16 I = src(srcX + 1, srcY + 1);

		uint16 Q = src(srcX - 2, srcY);
		uint16 R = src(srcX + 2, srcY);

            for (srcX = 0; srcX < width; ++srcX) {
			// Outputs
			uint16 J = E, K = E, L = E, M = E;

			if (((A ^ E) | (B ^ E) | (C ^ E) | (D ^ E) | (F ^ E) | (G ^ E) | (H ^ E) | (I ^ E)) != 0) {
				uint16 P = src(srcX, srcY - 2), S = src(srcX, srcY + 2);
				uint16 Bl = luma<bitFormat>(B), Dl = luma<bitFormat>(D), El = luma<bitFormat>(E), Fl = luma<bitFormat>(F), Hl = luma<bitFormat>(H);

				// 1:1 slope rules
				{
					if ((D == B && D != H && D != F) && (El >= Dl || E == A) && any_eq3(E, A, C, G) && ((El < Dl) || A != D || E != P || E != Q))
						J = D;
					if ((B == F && B != D && B != H) && (El >= Bl || E == C) && any_eq3(E, A, C, I) && ((El < Bl) || C != B || E != P || E != R))
						K = B;
					if ((H == D && H != F && H != B) && (El >= Hl || E == G) && any_eq3(E, A, G, I) && ((El < Hl) || G != H || E != S || E != Q))
						L = H;
					if ((F == H && F != B && F != D) && (El >= Fl || E == I) && any_eq3(E, C, G, I) && ((El < Fl) || I != H || E != R || E != S))
						M = F;
				}

				// Intersection rules
				{
					if ((E != F && all_eq4(E, C, I, D, Q) && all_eq2(F, B, H)) && (F != src(srcX + 3, srcY)))
						K = M = F;
					if ((E != D && all_eq4(E, A, G, F, R) && all_eq2(D, B, H)) && (D != src(srcX - 3, srcY)))
						J = L = D;
					if ((E != H && all_eq4(E, G, I, B, P) && all_eq2(H, D, F)) && (H != src(srcX, srcY + 3)))
						L = M = H;
					if ((E != B && all_eq4(E, A, C, H, S) && all_eq2(B, D, F)) && (B != src(srcX, srcY - 3)))
						J = K = B;
					if (Bl < El && all_eq4(E, G, H, I, S) && none_eq4(E, A, D, C, F))
						J = K = B;
					if (Hl < El && all_eq4(E, A, B, C, P) && none_eq4(E, D, G, I, F))
						L = M = H;
					if (Fl < El && all_eq4(E, A, D, G, Q) && none_eq4(E, B, C, I, H))
						K = M = F;
					if (Dl < El && all_eq4(E, C, F, I, R) && none_eq4(E, B, A, G, H))
						J = L = D;
				}

				// 2:1 slope rules
				{
					if (H != B) {
						if (H != A && H != E && H != C) {
							if (all_eq3(H, G, F, R) && none_eq2(H, D, src(srcX + 2, srcY - 1)))
								L = M;
							if (all_eq3(H, I, D, Q) && none_eq2(H, F, src(srcX - 2, srcY - 1)))
								M = L;
						}

						if (B != I && B != G && B != E) {
							if (all_eq3(B, A, F, R) && none_eq2(B, D, src(srcX + 2, srcY + 1)))
								J = K;
							if (all_eq3(B, C, D, Q) && none_eq2(B, F, src(srcX - 2, srcY + 1)))
								K = J;
						}
					} // H !== B

					if (F != D) {
						if (D != I && D != E && D != C) {
							if (all_eq3(D, A, H, S) && none_eq2(D, B, src(srcX + 1, srcY + 2)))
								J = L;
							if (all_eq3(D, G, B, P) && none_eq2(D, H, src(srcX + 1, srcY - 2)))
								L = J;
						}

						if (F != E && F != A && F != G) {
							if (all_eq3(F, C, H, S) && none_eq2(F, B, src(srcX - 1, srcY + 2)))
								K = M;
							if (all_eq3(F, I, B, P) && none_eq2(F, H, src(srcX - 1, srcY - 2)))
								M = K;
						}
					} // F !== D
				}     // 2:1 slope
			}
			
			int dstX = 2 * srcX;
			int dstY = 2 * srcY;

			dst(dstX, dstY, J);
			dst(dstX + 1, dstY, K);
			dst(dstX, dstY + 1, L);
			dst(dstX + 1, dstY + 1, M);

			A = B;
			B = C;
			C = src(srcX + 2, srcY - 1);
			Q = D;
			D = E;
			E = F;
			F = R;
			R = src(srcX + 3, srcY);
			G = H;
			H = I;
			I = src(srcX + 2, srcY + 1);
		} // X
	} // Y
}

void MMPX(const uint8 *srcPtr, uint32 srcPitch, uint8 *dstPtr, uint32 dstPitch, int width, int height) {
	extern int gBitFormat;
	if (gBitFormat == 565)
		MMPXTemplate<565>(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
	else
		MMPXTemplate<555>(srcPtr, srcPitch, dstPtr, dstPitch, width, height);
}
