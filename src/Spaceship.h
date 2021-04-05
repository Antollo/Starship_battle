#ifndef SPACESHIP_H_
#define SPACESHIP_H_

#include <utility>
#include <stdexcept>
#include "Object.h"
#include "Turret.h"
#include "Rock.h"
#include "Shield.h"

class Spaceship : public Object
{
public:
    static Spaceship *create(const std::string &type, const std::wstring &playerId, Object::ObjectId id = 0)
    {
        if (type.empty())
            throw std::runtime_error("Type of spaceship was empty.");
        if (playerId.empty())
            throw std::runtime_error("Commander ID was empty.");
        auto spaceship = new Spaceship(type, playerId, id);
        objects.emplace(spaceship->getId(), spaceship);
        return spaceship;
    }
    const Object::TypeId getTypeId() const override
    {
        return Object::TypeId::Spaceship;
    }
    Vec2f getCenterPosition() const override
    {
        body->GetMassData(const_cast<b2MassData *>(&massData));
        return getTransform().transformPoint(massData.center.x * worldScale, massData.center.y * worldScale);
    }
    Vec2f getLinearVelocity() const override
    {
        return Vec2f::asVec2f(body->GetLinearVelocity()) * worldScale;
    }
    float getAngularVelocity() const override
    {
        return body->GetAngularVelocity() * 180.f / pi;
    }
    float getReloadState()
    {
        return clock.getElapsedTime().asSeconds() / *reloadIt;
    }
    bool getAimState()
    {
        return aimState;
    }
    float getMaxHp() const
    {
        return maxHp;
    }
    float getHp() const
    {
        return hp;
    }
    void inceaseHp(float value)
    {
        hp += value;
    }
    void increaseArmor(float value)
    {
        armor += value;
    }
    const std::wstring &getPlayerId() const
    {
        return playerId;
    }
    const std::vector<std::pair<Vec2f, Vec2f>> &getEdges() const
    {
        static std::vector<std::pair<Vec2f, Vec2f>> ret;
        ret.resize(polygon.getVertexCount() - 1);
        for (std::size_t i = 0; i < polygon.getVertexCount() - 1; i++)
        {
            ret[i].first = Vec2f::asVec2f(getTransform().transformPoint(polygon[i].position) / worldScale);
            ret[i].second = Vec2f::asVec2f(getTransform().transformPoint(polygon[i + 1].position) / worldScale);
        }
        return ret;
    }
    ~Spaceship() override
    {
        body->SetUserData(nullptr);
        decltype(Object::objects)::iterator it, jt;
        for (auto it = Object::objects.begin(); it != Object::objects.end();)
        {
            jt = it++;
            if (jt->second->getId() == getId())
                jt->second->destroy = true;
        }

        for (const auto &id : shields)
        {
            if (Object::objects.count(id))
                Object::objects[id]->destroy = true;
        }

        Rock::create(polygon, body);
    }
    void process(float delta) override
    {
        body->GetMassData(&massData);
        setOrigin(massData.center.x * worldScale, massData.center.y * worldScale);
        setPosition(body->GetPosition().x * worldScale, body->GetPosition().y * worldScale);
        setRotation(body->GetAngle() * 180.f / pi);
        setOrigin(0, 0);
        if (forward)
            onForward();
        if (left)
            onLeft();
        if (right)
            onRight();
        onAim(delta);
        if (shoot)
            onShoot();
        if (hp < 0.f)
            destroy = true;
    }
    bool forward, left, right, shoot;
    Vec2f aimCoords;

private:
    Spaceship(const std::string &type, const std::wstring &newPlayerId = L"AutomatedPilot-" + std::to_wstring(counter + 1), ObjectId objectId = 0);
    void draw(RenderSerializerBase &target, sf::RenderStates states, const Vec2f &position, const Vec2f &linearVelocity, float angularVelocity) const noexcept override
    {
        states.transform *= getTransform();
        Vec2f myLinearVelocity = getLinearVelocity();
        Vec2f myPosition = getCenterPosition();
        float myAngularVelocity = getAngularVelocity();
        target.draw(polygon, states, myPosition, myLinearVelocity, myAngularVelocity);
        for (const Turret &turret : turrets)
            target.draw(turret, states, myPosition, myLinearVelocity, myAngularVelocity);
    }
    void onForward() noexcept
    {
        body->ApplyForceToCenter(b2Vec2(cosf(body->GetAngle()) * force, sinf(body->GetAngle()) * force), true);
    }
    void onLeft() noexcept
    {
        body->ApplyTorque(-torque, true);
    }
    void onRight() noexcept
    {
        body->ApplyTorque(torque, true);
    }
    inline float piPi(float x)
    {
        x = std::fmod(x + pi, 2 * pi);
        if (x < 0)
            x += 2 * pi;
        return x - pi;
    }
    void onAim(float delta) noexcept
    {
        sf::Vector2f targetRelativeToTurret;
        aimState = false;
        for (Turret &turret : turrets)
        {
            targetRelativeToTurret = getInverseTransform().transformPoint(aimCoords) - turret.getPosition();

            float dest = std::atan2(targetRelativeToTurret.y, targetRelativeToTurret.x);
            aimState |= turret.setRotation(dest * 180.f / pi, delta);
        }
    }
    virtual void onShoot() noexcept
    {
        if (clock.getElapsedTime().asSeconds() > *reloadIt)
        {
            for (Turret &turret : turrets)
                turret.shoot(getTransform(), getRotation() / 180.f * pi, Vec2f::asVec2f(body->GetLinearVelocity()), -getId());
            clock.restart();
            reloadIt++;
            if (reloadIt == reload.end())
                reloadIt = reload.begin();
        }
    }
    std::wstring playerId;
    b2Body *body;
    b2MassData massData;
    float force, torque, maxHp, hp, armor;
    std::vector<float> reload;
    std::vector<float>::iterator reloadIt;
    bool aimState;
    std::vector<Turret> turrets;
    std::vector<Object::ObjectId> shields;
    sf::Clock clock;
    friend class Bot;
    friend class ContactListener;
    friend class RenderSerializer;
};

#endif
