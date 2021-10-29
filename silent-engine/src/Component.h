#pragma once
#include <memory>

class Entity;

class Component {
public:
	virtual void update(float deltaTime);
	
    void setEntity(std::shared_ptr<Entity> entity);

protected:	
    std::weak_ptr<Entity> _entity;

private:
};
