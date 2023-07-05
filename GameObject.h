#pragma once
#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

// Abstract base class representing an interface
class GameObject {
public:
    // Pure virtual function to be implemented by derived classes
    virtual void onRender() const = 0;
    virtual void onKeyDown(int keyCode) const = 0;
    virtual void onMouseKeyDown(int mouseKeyCode) const = 0;
};

#endif