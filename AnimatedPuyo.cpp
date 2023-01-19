/* FloboPuyo
 * Copyright (C) 2004
 *   Florent Boudet        <flobo@ios-software.com>,
 *   Jean-Christophe Hoelt <jeko@ios-software.com>,
 *   Guillaume Borios      <gyom@ios-software.com>
 *
 * iOS Software <http://www.ios-software.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 *
 */
 
#include "AnimatedPuyo.h"
#include "PuyoView.h"

// crade, mais basta
extern IIM_Surface *puyoCircle[32];
extern IIM_Surface *puyoEye[3];

AnimatedPuyo::AnimatedPuyo(PuyoState state, PuyoView *attachedView) : PuyoPuyo(state)
{
    puyoEyeState = random() % 700;
    visibilityFlag = true;
    this->attachedView = attachedView;
}

AnimatedPuyo::~AnimatedPuyo()
{
    while (!animationQueue.empty())
           removeCurrentAnimation();
}

void AnimatedPuyo::addAnimation(PuyoAnimation *animation)
{
    animationQueue.push_back(animation);
}

PuyoAnimation * AnimatedPuyo::getCurrentAnimation() const
{
    if (animationQueue.empty())
        return NULL;
    return animationQueue.at(0);
}

void AnimatedPuyo::removeCurrentAnimation()
{
    if (animationQueue.empty())
        return;
    delete animationQueue.at(0);
    animationQueue.erase(animationQueue.begin());
}

void AnimatedPuyo::cycleAnimation()
{
    smallTicksCount+=2;
    PuyoAnimation * animation = getCurrentAnimation();
    if ((animation != NULL)) {
        animation->cycle();
        if (animation->isFinished()) {
            removeCurrentAnimation();
        }
    }
}

bool AnimatedPuyo::isRenderingAnimation() const
{
    PuyoAnimation *animation = getCurrentAnimation();
    if (animation == NULL)
        return false;
    if (animation->isFinished())
        return false;
    return animation->isEnabled();
}

void AnimatedPuyo::render()
{
    SDL_Painter &painter = attachedView->getPainter();
    
    puyoEyeState++;
    if (attachedView == NULL)
        return;
    if (!visibilityFlag)
        return;
    PuyoGame *attachedGame = attachedView->getAttachedGame();
    
    bool falling  = attachedGame->getFallingState() < PUYO_EMPTY;
    
    SDL_Rect drect;
    PuyoAnimation *animation = getCurrentAnimation();
    if (!isRenderingAnimation()) {
        IIM_Surface *currentSurface = attachedView->getSurfaceForPuyo(this);
        if (currentSurface != NULL) {
            drect.x = getScreenCoordinateX();
            drect.y = getScreenCoordinateY();
            if (this->getPuyoState() < PUYO_EMPTY)
                drect.y -= attachedGame->getSemiMove() * TSIZE / 2;
            drect.w = currentSurface->w;
            drect.h = currentSurface->h;
            painter.requestDraw(currentSurface, &drect);
            
            /* Main puyo show */
            if (falling && (this == attachedGame->getFallingPuyo()))
                painter.requestDraw(puyoCircle[(smallTicksCount >> 2) & 0x1F], &drect);
            
            /* Eye management */
            if (getPuyoState() != PUYO_NEUTRAL) {
                while (puyoEyeState >= 750) puyoEyeState -= 750;
                int eyePhase = puyoEyeState;
                if (eyePhase < 5)
                    painter.requestDraw(puyoEye[1], &drect);
                else if (eyePhase < 15)
                    painter.requestDraw(puyoEye[2], &drect);
                else if (eyePhase < 20)
                    painter.requestDraw(puyoEye[1], &drect);
                else
                    painter.requestDraw(puyoEye[0], &drect);
            }
        }
    }
    else {
        if (!animation->isFinished()) {
            animation->draw(attachedGame->getSemiMove());
        }
    }
}

int AnimatedPuyo::getScreenCoordinateX() const
{
    return attachedView->getScreenCoordinateX(getPuyoX());
}

int AnimatedPuyo::getScreenCoordinateY() const
{
    return attachedView->getScreenCoordinateY(getPuyoY());
}




AnimatedPuyoFactory::AnimatedPuyoFactory(PuyoView *attachedView)
{
    this->attachedView = attachedView;
}

AnimatedPuyoFactory::~AnimatedPuyoFactory()
{
    while (!puyoWalhalla.empty()) {
        delete puyoWalhalla.at(0);
        puyoWalhalla.erase(puyoWalhalla.cbegin());
    }
}

PuyoPuyo *AnimatedPuyoFactory::createPuyo(PuyoState state)
{
    return new AnimatedPuyo(state, attachedView);
}

void AnimatedPuyoFactory::deletePuyo(PuyoPuyo *target)
{
    puyoWalhalla.push_back(target);
}


void AnimatedPuyoFactory::renderWalhalla()
{
    for (int i = puyoWalhalla.size() - 1 ; i >= 0 ; i--) {
        AnimatedPuyo *currentPuyo = (AnimatedPuyo *)(puyoWalhalla.at(i));
        currentPuyo->render();
    }
}

void AnimatedPuyoFactory::cycleWalhalla()
{
    for (int i = puyoWalhalla.size() - 1 ; i >= 0 ; i--) {
        AnimatedPuyo *currentPuyo = (AnimatedPuyo *)(puyoWalhalla.at(i));
        if (currentPuyo->getCurrentAnimation() != NULL) {
            currentPuyo->cycleAnimation();
        } else {
            puyoWalhalla.erase(puyoWalhalla.cbegin() + i);
            delete currentPuyo;
        }
    }
}

