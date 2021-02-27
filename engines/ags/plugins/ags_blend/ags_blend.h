/* ScummVM - Graphic Adventure Engine
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 */

#ifndef AGS_PLUGINS_AGS_BLEND_AGS_BLEND_H
#define AGS_PLUGINS_AGS_BLEND_AGS_BLEND_H

#include "ags/plugins/plugin_base.h"

namespace AGS3 {
namespace Plugins {
namespace AGSBlend {

/**
 * Author: Steven Poulton
 * Description: An AGS Plugin to allow true Alpha Blending
 */
class AGSBlend : public PluginBase {
private:
	static IAGSEngine *_engine;
private:
	static const char *AGS_GetPluginName();
	static void AGS_EngineStartup(IAGSEngine *lpEngine);

private:
	/**
	 * Gets the alpha value at coords x,y
	 */
	static NumberPtr GetAlpha(const ScriptMethodParams &params);

	/**
	 * Sets the alpha value at coords x,y
	 */
	static NumberPtr PutAlpha(const ScriptMethodParams &params);

	/**
	 * Translates index from a 2D array to a 1D array
	 */
	static int xytolocale(int x, int y, int width);

	static NumberPtr HighPass(const ScriptMethodParams &params);
	static NumberPtr Blur(const ScriptMethodParams &params);
	static NumberPtr DrawSprite(const ScriptMethodParams &params);
	static NumberPtr DrawAdd(const ScriptMethodParams &params);
	static NumberPtr DrawAlpha(const ScriptMethodParams &params);

public:
	AGSBlend();
};

} // namespace AGSBlend
} // namespace Plugins
} // namespace AGS3

#endif
