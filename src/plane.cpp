/*
** plane.cpp
**
** This file is part of mkxp.
**
** Copyright (C) 2013 Jonas Kulla <Nyocurio@gmail.com>
**
** mkxp is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** mkxp is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with mkxp.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "plane.h"

#include "sharedstate.h"
#include "bitmap.h"
#include "etc.h"
#include "etc-internal.h"
#include "util.h"

#include "gl-util.h"
#include "quad.h"
#include "quadarray.h"
#include "transform.h"
#include "etc-internal.h"
#include "shader.h"
#include "glstate.h"

#include <sigc++/connection.h>

static float fwrap(float value, float range)
{
	float res = fmod(value, range);
	return res < 0 ? res + range : res;
}

struct PlanePrivate
{
	Bitmap *bitmap;

	Rect *srcRect;

	NormValue opacity;
	BlendType blendType;
	Color *color;
	Tone *tone;

	int ox, oy;
	float zoomX, zoomY;

	float waterTime;

	Scene::Geometry sceneGeo;

	bool quadSourceDirty;

	SimpleQuadArray qArray;

	EtcTemps tmp;

	sigc::connection prepareCon;
	sigc::connection srcRectCon;

	PlanePrivate()
	    : bitmap(0),
	      srcRect(&tmp.rect),
	      opacity(255),
	      blendType(BlendNormal),
	      color(&tmp.color),
	      tone(&tmp.tone),
	      ox(0), oy(0),
	      zoomX(1), zoomY(1),
	      quadSourceDirty(false),
		  waterTime(0.0)
	{
		updateSrcRectCon();
		prepareCon = shState->prepareDraw.connect
		        (sigc::mem_fun(this, &PlanePrivate::prepare));

		qArray.resize(1);
	}

	~PlanePrivate()
	{
		srcRectCon.disconnect();
		prepareCon.disconnect();
	}

	void onSrcRectChange()
	{
		quadSourceDirty = true;
	}

	void updateSrcRectCon()
	{
		/* Cut old connection */
		srcRectCon.disconnect();
		/* Create new one */
		srcRectCon = srcRect->valueChanged.connect
				(sigc::mem_fun(this, &PlanePrivate::onSrcRectChange));
	}

	void updateQuadSource()
	{
		if (nullOrDisposed(bitmap))
			return;

		if (gl.npot_repeat && srcRect->toIntRect() == bitmap->rect())
		{
			FloatRect srcRect;
			srcRect.x = (sceneGeo.orig.x + ox) / zoomX;
			srcRect.y = (sceneGeo.orig.y + oy) / zoomY;
			srcRect.w = sceneGeo.rect.w / zoomX;
			srcRect.h = sceneGeo.rect.h / zoomY;

			Quad::setTexRect(&qArray.vertices[0], srcRect);
			qArray.commit();

			return;
		}

		/* Scaled (zoomed) bitmap dimensions */
		float sw = srcRect->width  * zoomX;
		float sh = srcRect->height * zoomY;

		/* Plane offset wrapped by scaled bitmap dims */
		float wox = fwrap(ox, sw);
		float woy = fwrap(oy, sh);

		/* Viewport dimensions */
		int vpw = sceneGeo.rect.w;
		int vph = sceneGeo.rect.h;

		/* Amount the scaled bitmap is tiled (repeated) */
		size_t tilesX = ceil((vpw - sw + wox) / sw) + 1;
		size_t tilesY = ceil((vph - sh + woy) / sh) + 1;

		FloatRect tex = srcRect->toFloatRect();

		qArray.resize(tilesX * tilesY);

		for (size_t y = 0; y < tilesY; ++y)
			for (size_t x = 0; x < tilesX; ++x)
			{
				SVertex *vert = &qArray.vertices[(y*tilesX + x) * 4];
				FloatRect pos(x*sw - wox, y*sh - woy, sw, sh);

				Quad::setTexPosRect(vert, tex, pos);
			}

		qArray.commit();
	}

	void prepare()
	{
		if (quadSourceDirty)
		{
			updateQuadSource();
			quadSourceDirty = false;
		}
	}
};

Plane::Plane(Viewport *viewport)
    : ViewportElement(viewport)
{
	p = new PlanePrivate();

	onGeometryChange(scene->getGeometry());
}

DEF_ATTR_RD_SIMPLE(Plane, Bitmap,    Bitmap*, p->bitmap)
DEF_ATTR_RD_SIMPLE(Plane, OX,        int,     p->ox)
DEF_ATTR_RD_SIMPLE(Plane, OY,        int,     p->oy)
DEF_ATTR_RD_SIMPLE(Plane, ZoomX,     float,   p->zoomX)
DEF_ATTR_RD_SIMPLE(Plane, ZoomY,     float,   p->zoomY)
DEF_ATTR_RD_SIMPLE(Plane, BlendType, int,     p->blendType)

DEF_ATTR_SIMPLE(Plane, SrcRect,   Rect&,  *p->srcRect)
DEF_ATTR_SIMPLE(Plane, Opacity,   int,     p->opacity)
DEF_ATTR_SIMPLE(Plane, Color,     Color&, *p->color)
DEF_ATTR_SIMPLE(Plane, Tone,      Tone&,  *p->tone)
DEF_ATTR_SIMPLE(Plane, WaterTime, float, p->waterTime)

Plane::~Plane()
{
	dispose();
}

void Plane::setBitmap(Bitmap *value)
{
	guardDisposed();

	p->bitmap = value;

	if (!value)
		return;

	value->ensureNonMega();

	*p->srcRect = value->rect();
	p->onSrcRectChange();
}

void Plane::setOX(int value)
{
	guardDisposed();

	if (p->ox == value)
	        return;

	p->ox = value;
	p->quadSourceDirty = true;
}

void Plane::setOY(int value)
{
	guardDisposed();

	if (p->oy == value)
	        return;

	p->oy = value;
	p->quadSourceDirty = true;
}

void Plane::setZoomX(float value)
{
	guardDisposed();

	if (p->zoomX == value)
	        return;

	p->zoomX = value;
	p->quadSourceDirty = true;
}

void Plane::setZoomY(float value)
{
	guardDisposed();

	if (p->zoomY == value)
	        return;

	p->zoomY = value;
	p->quadSourceDirty = true;
}

void Plane::setBlendType(int value)
{
	guardDisposed();

	switch (value)
	{
	default :
	case BlendNormal :
		p->blendType = BlendNormal;
		return;
	case BlendAddition :
		p->blendType = BlendAddition;
		return;
	case BlendSubstraction :
		p->blendType = BlendSubstraction;
		return;
	}
}

void Plane::initDynAttribs()
{
	p->srcRect = new Rect;
	p->color = new Color;
	p->tone = new Tone;

	p->updateSrcRectCon();
}

void Plane::draw()
{
	if (nullOrDisposed(p->bitmap))
		return;

	if (!p->opacity)
		return;

	ShaderBase *base;

	if (p->waterTime != 0)
	{
		WaterShader &shader = shState->shaders().water;
		shader.bind();
		shader.applyViewportProj();
		shader.setiTime(p->waterTime);
		shader.setOpacity(p->opacity.norm);

		base = &shader;
	}
	else if (p->color->hasEffect() || p->tone->hasEffect() || p->opacity != 255)
	{
		PlaneShader &shader = shState->shaders().plane;

		shader.bind();
		shader.applyViewportProj();
		shader.setTone(p->tone->norm);
		shader.setColor(p->color->norm);
		shader.setFlash(Vec4());
		shader.setOpacity(p->opacity.norm);

		base = &shader;
	}
	else
	{
		SimpleShader &shader = shState->shaders().simple;

		shader.bind();
		shader.applyViewportProj();
		shader.setTranslation(Vec2i());

		base = &shader;
	}

	glState.blendMode.pushSet(p->blendType);

	p->bitmap->bindTex(*base);

	if (gl.npot_repeat)
		TEX::setRepeat(true);

	p->qArray.draw();

	if (gl.npot_repeat)
		TEX::setRepeat(false);

	glState.blendMode.pop();
}

void Plane::onGeometryChange(const Scene::Geometry &geo)
{
	if (gl.npot_repeat)
		Quad::setPosRect(&p->qArray.vertices[0], FloatRect(geo.rect));

	p->sceneGeo = geo;
	p->quadSourceDirty = true;
}

void Plane::releaseResources()
{
	unlink();

	delete p;
}
