// Particle Classes    by Alan Baylis 2001

// Inspired by the work by
// John van der Burg
// Mystic Game Development
// john@mysticgd.com

#ifndef PARTICLE_H
#define PARTICLE_H

#include "vector.h"
//#include "listnode.h"
#include "tll.h"

enum ParticleType{spark, roman, bouncy};   // Particle types

typedef struct ParticleInfo
{
    bool Alive;
    int Leaf;
    VECTOR Pos;
    VECTOR OldPos;
    VECTOR OrigPos;
    VECTOR Velocity;
    VECTOR Vertex[4];
    float Color[4];
    float Energy;
    float SizeX;
    float SizeY;
} ParticleInfo;

typedef struct SystemInfo
{
    int Id;
    int numAlive;
    bool Visibility;
    int numParticles;
    ParticleType Type;
    int BlendMode;
    unsigned int TexID;
    float Color[4];
    VECTOR Pos;
    VECTOR Normal;
    VECTOR InitialVelocity;
} SystemInfo;

class PARTICLE
{
    public:
        PARTICLE(){};
        ~PARTICLE(){};

        int Compare(const PARTICLE& Particle);
        int GetMyPosition() const {return linkPosition;}
        void SetMyPosition(int newPosition) {linkPosition = newPosition;}
        int linkPosition;

        ParticleInfo PartInfo;
};

class ParticleSystem
{
    public:
        ParticleSystem(){};
        ~ParticleSystem(){};

        int Compare(const ParticleSystem& ParticleSys);
        int GetMyPosition() const {return linkPosition;}
        void SetMyPosition(int newPosition) {linkPosition = newPosition;}
        int linkPosition;

        LinkedList<PARTICLE> ParticleList;
        SystemInfo SysInfo;

        int GetNumAlive();
        void SetupParticles();
        PARTICLE* Add();
        void Remove();

        virtual void SetDefaults(PARTICLE* Particle);
        virtual void SetShape(PARTICLE* Particle);
        virtual void Update();
        virtual void Render(int nodeid);
};

//class ParticleManager
//{
//    public:
//        ParticleManager(){numSystems = 0;}
//        ~ParticleManager(){};
//
//        int numSystems;
//        LinkedList<ParticleSystem> SystemList;
//
//        void SetVisibility(int Id, bool State);
//        void ToggleVisibility(int Id);
//        void SetType(int Id, ParticleType Type);
//        void SetBlendMode(int Id, int BlendMode);
//        void SetTextureId(int Id, unsigned int TexID);
//        void SetId(int Id, int newId);
//        void Remove(int Id);
//        void RemoveType(ParticleType Type);
//        void Update();
//        void Render(int nodeid);
//        ParticleSystem* Add(ParticleSystem* PartSys);
//};

// Derived Particle Systems

// spark type
class Spark : public ParticleSystem
{
    public:
        Spark(){};
        ~Spark(){};

        // Override base methods
        void Update();
        void Render(int nodeid);
        void SetShape(PARTICLE* Particle);
        void SetDefaults(PARTICLE* Particle);
};


void RenderParticles(int nodeid);
ParticleSystem* CreateSparks(VECTOR OldPos, VECTOR Pos, VECTOR Normal, float radius);

#endif // PARTICLE_H
