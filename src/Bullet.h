#ifndef BULLET_H_
#define BULLET_H_

#include "Object.h"

class Bullet : public Object, public sf::Transformable
{
public:
    static Bullet* create(const std::vector<Vec2f>& points, Vec2f position, const float& angle, const float& penetrationAngle, const Vec2f& velocity, const int& index)
    {
        objects.emplace_back(new Bullet(points, position, angle, penetrationAngle, velocity, index));
        return dynamic_cast<Bullet*>(objects.back().get());
    }
    const Object::typeId getTypeId() const noexcept override
    {
        return Object::typeId::Bullet;
    }
    ~Bullet() override
    {
        world.DestroyBody(body);
    }
    void process() override
    {
        body->GetMassData(&massData);
        setOrigin(massData.center.x * worldScale, massData.center.y * worldScale);
        setPosition(body->GetPosition().x * worldScale, body->GetPosition().y * worldScale);
        setRotation(body->GetAngle() * 180.f / pi);
        setOrigin(0, 0);
		destroy = destroy || body->GetLinearVelocity().LengthSquared() < 0.5f;
        Object::process();
    }
private:
    Bullet(const std::vector<Vec2f>& points, const Vec2f& position, const float& angle, const float& penetrationAngle, const Vec2f& velocity, const int& index) : minAngle(penetrationAngle)
    {
        b2BodyDef bodyDef;
        bodyDef.position = position;
        bodyDef.angle = angle;
        bodyDef.type = b2_dynamicBody;
        bodyDef.linearDamping = 0.1f;
        bodyDef.angularDamping = 0.1f;
        bodyDef.bullet = true;
        bodyDef.userData = this;

        body = world.CreateBody(&bodyDef);

        b2PolygonShape shape;
        shape.Set(reinterpret_cast<const b2Vec2*>(points.data()), points.size());

        b2FixtureDef fixtureDef;
        fixtureDef.density = 10;
        fixtureDef.friction = 0.5;
        fixtureDef.filter.groupIndex = index;
        fixtureDef.shape = &shape;
        body->CreateFixture(&fixtureDef);

        polygon.resize(points.size() + 1);
        polygon.setPrimitiveType(sf::PrimitiveType::LineStrip);
        for (std::size_t i = 0; i < points.size(); i++)
        {
            polygon[i].position = (*reinterpret_cast<const sf::Vector2f*>(&points[i])) * worldScale;
            polygon[i].color = sf::Color::White;
        }
        polygon[points.size()] = polygon[0];

        body->SetLinearVelocity(velocity);
        process();
    }
    void draw(sf::RenderTarget& target, sf::RenderStates states) const noexcept override
    {
        states.transform *= getTransform();
        target.draw(polygon, states);
    }
    inline int getID()
    {
        return -body->GetFixtureList()[0].GetFilterData().groupIndex;
    }
    sf::VertexArray polygon;
    b2Body* body;
    b2MassData massData;
    float minAngle;
    friend class ContactListener;
};

#endif
