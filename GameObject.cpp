#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H
class GameObject {
public:
	virtual void onRender() const = 0;
	void OnKey(int keyCode) {};
	void onMouseKey(int mouseKeyCode) {};

};
#endif