// Particle Classes    by Alan Baylis 2001

// Inspired by the work by
// John van der Burg
// Mystic Game Development
// john@mysticgd.com

#include "particle.h"
#include "matrix.h"
#include "texture.h"
//#include "listnode.h"
//#include "collision.h"
#include "general.h"
//#include "bsp.h"
//#include "locmath.h"


//extern BSP_node* root;
//extern ParticleManager PManager;
extern TEXTURE* texture;
//extern LinkedList<ListNode> LeafList;
//extern LinkedList<ListNode> PartitionList;
//extern ListNode* listnode;

int PARTICLE::Compare(const PARTICLE& Particle)
{
    if (linkPosition < Particle.linkPosition)
        return smaller;
    if (linkPosition > Particle.linkPosition)
        return bigger;
    else
        return same;
}

// ParticleSystem Methods
void ParticleSystem::Render(int nodeid)
{
   // Add a default method
}

void ParticleSystem::Update()
{
   // Add a default method
}

void ParticleSystem::SetDefaults(PARTICLE* Particle)
{
   // Add a default method
}

void ParticleSystem::SetShape(PARTICLE* Particle)
{
   // Add a default method
}

int ParticleSystem::GetNumAlive()
{
    int numParticles = 0;
    PARTICLE* tempParticle;
    for (int loop = 1; loop <= SysInfo.numParticles; loop++)
    {
        tempParticle = ParticleList.Get(loop);
        if (tempParticle->PartInfo.Alive)
            numParticles++;
    }
    return numParticles;
}

// Add initial particles to the empty list
void ParticleSystem::SetupParticles()
{
    for (int loop = 1; loop <= SysInfo.numParticles; loop++)
    {
        PARTICLE* newParticle = new PARTICLE;
        SetDefaults(newParticle);
        SetShape(newParticle);
        newParticle->linkPosition = loop;
        ParticleList.Insert(newParticle);
    }
}

void ParticleSystem::Remove()
{
    if (SysInfo.numParticles > 0)
    {
        ParticleList.Delete(1);
        --SysInfo.numParticles;
    }
}

PARTICLE* ParticleSystem::Add()
{
    PARTICLE* newParticle = new PARTICLE;
    SetDefaults(newParticle);
    SetShape(newParticle);
    newParticle->linkPosition = ++SysInfo.numParticles;
    ParticleList.Insert(newParticle);
    return newParticle;
}

int ParticleSystem::Compare(const ParticleSystem& ParticleSys)
{
  if (linkPosition < ParticleSys.linkPosition)
    return smaller;
  if (linkPosition > ParticleSys.linkPosition)
    return bigger;
  else
    return same;
}
//
//
//// Particle Manager Methods
//void ParticleManager::Update()
//{
//    int loop, innerloop;
//    ParticleSystem* PartSys;
//    PARTICLE* tempParticle;
//    for (loop = 1; loop <= numSystems; loop++)
//    {
//        PartSys = SystemList.Get(loop);
//
//        if (!PartSys->GetNumAlive())
//        {
//            SystemList.Delete(PartSys->linkPosition);
//            numSystems--;
//        }
//        else
//        {
//            PartSys->Update();
//            for (innerloop = 1; innerloop < PartSys->SysInfo.numParticles; innerloop++)
//            {
//                tempParticle = PartSys->ParticleList.Get(innerloop);
//                tempParticle->PartInfo.Leaf = FindCurrentLeaf(tempParticle->PartInfo.Pos, root);
//            }
//        }
//    }
//}
//
//void ParticleManager::Render(int nodeid)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        PartSys->Render(nodeid);
//    }
//}
//
//void ParticleManager::SetId(int Id, int newId)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        if (Id == PartSys->SysInfo.Id)
//            PartSys->SysInfo.Id = newId;
//    }
//}
//
//void ParticleManager::SetTextureId(int Id, unsigned int TexID)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        if (Id == PartSys->SysInfo.Id)
//            PartSys->SysInfo.TexID = TexID;
//    }
//}
//
//void ParticleManager::SetBlendMode(int Id, int BlendMode)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        if (Id == PartSys->SysInfo.Id)
//            PartSys->SysInfo.BlendMode = BlendMode;
//    }
//}
//
//void ParticleManager::SetType(int Id, ParticleType Type)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        if (Id == PartSys->SysInfo.Id)
//            PartSys->SysInfo.Type = Type;
//    }
//}
//
//void ParticleManager::SetVisibility(int Id, bool State)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        if (Id == PartSys->SysInfo.Id)
//            PartSys->SysInfo.Visibility = State;
//    }
//}
//
//void ParticleManager::ToggleVisibility(int Id)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        if (Id == PartSys->SysInfo.Id)
//            PartSys->SysInfo.Visibility = !PartSys->SysInfo.Visibility;
//    }
//}
//
//ParticleSystem* ParticleManager::Add(ParticleSystem* PartSys)
//{
//    PartSys->SetupParticles();
//    PartSys->linkPosition = ++numSystems;
//    SystemList.Insert(PartSys);
//    return PartSys;
//}
//
//void ParticleManager::Remove(int Id)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        if (Id == PartSys->SysInfo.Id)
//        {
//            SystemList.Delete(PartSys->linkPosition);            
//            --numSystems;
//        }
//    }
//}
//
//void ParticleManager::RemoveType(ParticleType Type)
//{
//    for (int loop = 1; loop <= numSystems; loop++)
//    {
//        ParticleSystem* PartSys = SystemList.Get(loop);
//        if (Type == PartSys->SysInfo.Type)
//        {
//            SystemList.Delete(PartSys->linkPosition);
//            --numSystems;
//        }
//    }
//}

// Derived Particle Systems

// spark type
void Spark::Update()
{
    //bool CollisionFlag;
    VECTOR VelocityVector, normal, pos, oldpos, temppos;
    PARTICLE* tempParticle;
    // Loop through all the particles of this system
    for (int loop = 1; loop <= SysInfo.numParticles; loop++)
    {
        // Get the particle from the list
        tempParticle = ParticleList.Get(loop);
        // Set the old position
        tempParticle->PartInfo.OldPos = tempParticle->PartInfo.Pos;
        // Apply gravity
        tempParticle->PartInfo.Velocity.x += 0.0;
        tempParticle->PartInfo.Velocity.y += -0.002;
        tempParticle->PartInfo.Velocity.z += 0.0;

        if (tempParticle->PartInfo.Energy > 0.0)
        {
            // Set the alpha channel to the energy value
            tempParticle->PartInfo.Color[3] = tempParticle->PartInfo.Energy;
            // Decrease the energy
            tempParticle->PartInfo.Energy -= 0.002;
            // Update the position
            oldpos = tempParticle->PartInfo.Pos;
            pos = tempParticle->PartInfo.Pos + tempParticle->PartInfo.Velocity;
            temppos = pos;
            // Check for a collision and get the normal of the collision polygon
            //CollisionFlag = CheckForParticleCollision(tempParticle, oldpos, &temppos, &normal);
            // If there was a collision then reflect the velocity vector
            //if (CollisionFlag)
            //{
            //    VECTOR vectn = normal * (normal.dot(tempParticle->PartInfo.Velocity));
            //    VECTOR vectt = tempParticle->PartInfo.Velocity - vectn;
            //    VECTOR vel = (vectt - vectn);
            //    tempParticle->PartInfo.Pos.x += vel.x;
            //    tempParticle->PartInfo.Pos.y += vel.y;
            //    tempParticle->PartInfo.Pos.z += vel.z;
            //    // Decrease the velocity
            //    tempParticle->PartInfo.Velocity.x = vel.x / 3.0;
            //    tempParticle->PartInfo.Velocity.y = vel.y / 3.0;
            //    tempParticle->PartInfo.Velocity.z = vel.z / 3.0;
            //    // Reduce the particles energy due to the collision
            //    tempParticle->PartInfo.Energy -= 0.2;
            //}
            //else // Update the position as normal
            {
                tempParticle->PartInfo.Pos.x += tempParticle->PartInfo.Velocity.x;
                tempParticle->PartInfo.Pos.y += tempParticle->PartInfo.Velocity.y;
                tempParticle->PartInfo.Pos.z += tempParticle->PartInfo.Velocity.z;
            }
        }
        else // The particles energy has reduced to zero
        {
            tempParticle->PartInfo.Alive = false;
        }
    }
}

void Spark::Render(int nodeid)
{
    MATRIX mat;
    VECTOR up;
    VECTOR right;
    PARTICLE* tempParticle;
    // Get the current modelview matrix
    glGetFloatv(GL_MODELVIEW_MATRIX, mat.Element);
    // Set texture states


	glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glDepthMask(0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDisable(GL_LIGHTING);
    glBindTexture(GL_TEXTURE_2D, SysInfo.TexID);
    // Loop through all the particles in this system
    for (int loop = 1; loop <= SysInfo.numParticles; loop++)
    {
        // Get the particle from the list
        tempParticle = ParticleList.Get(loop);
        // If the particle is in the current bsp node
        //if (tempParticle->PartInfo.Leaf == nodeid)
        {
            // If the particle is still alive
            if (tempParticle->PartInfo.Alive)
            {
                // Set the shape, should really be performed in Spark::SetShape()
                right.x = mat.Element[0];
                right.y = mat.Element[4];
                right.z = mat.Element[8];
                right.Normalize();
                right.x *= tempParticle->PartInfo.SizeX / 2;
                right.y *= tempParticle->PartInfo.SizeX / 2;
                right.z *= tempParticle->PartInfo.SizeX / 2;

                up.x = mat.Element[1];
                up.y = mat.Element[5];
                up.z = mat.Element[9];
                up.Normalize();
                up.x *= tempParticle->PartInfo.SizeY / 2;
                up.y *= tempParticle->PartInfo.SizeY / 2;
                up.z *= tempParticle->PartInfo.SizeY / 2;
                // Set the color to the default values
                glColor4f(tempParticle->PartInfo.Color[0],
                            tempParticle->PartInfo.Color[1],
                            tempParticle->PartInfo.Color[2],
                            tempParticle->PartInfo.Color[3]);
                // Render the billboarded particle
                glBegin(GL_QUADS);
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex3f(tempParticle->PartInfo.Pos.x + (-right.x - up.x),
                                tempParticle->PartInfo.Pos.y + (-right.y - up.y),
                                tempParticle->PartInfo.Pos.z + (-right.z - up.z));
                    glTexCoord2f(1.0f, 0.0f);
                    glVertex3f(tempParticle->PartInfo.Pos.x + (right.x - up.x),
                                tempParticle->PartInfo.Pos.y + (right.y - up.y),
                                tempParticle->PartInfo.Pos.z + (right.z - up.z));
                    glTexCoord2f(1.0f, 1.0f);
                    glVertex3f(tempParticle->PartInfo.Pos.x + (right.x + up.x),
                                tempParticle->PartInfo.Pos.y + (right.y + up.y),
                                tempParticle->PartInfo.Pos.z + (right.z + up.z));
                    glTexCoord2f(0.0f, 1.0f);
                    glVertex3f(tempParticle->PartInfo.Pos.x + (up.x - right.x),
                                tempParticle->PartInfo.Pos.y + (up.y - right.y),
                                tempParticle->PartInfo.Pos.z + (up.z - right.z));
                glEnd();
            }
        }
    }
    // Reset texture states
    glEnable(GL_LIGHTING);
    glDepthMask(1);
    glDisable(GL_BLEND);
}

void Spark::SetDefaults(PARTICLE* Particle)
{
    // Set this particle systems properties
    Particle->PartInfo.Alive = true;

    Particle->PartInfo.Pos = SysInfo.Pos;
    Particle->PartInfo.OldPos = SysInfo.Pos;
    Particle->PartInfo.OrigPos = SysInfo.Pos;
    // Reflect the initial velocity
    VECTOR Velocity = SysInfo.InitialVelocity;
    VECTOR vectn = SysInfo.Normal * (SysInfo.Normal.dot(Velocity));
    VECTOR vectt = Velocity - vectn;
    VECTOR vel = (vectt - vectn);
    Velocity.Normalize();
    // Create a random spread for each particle
    float x;
    x = (float)rand()/(float)RAND_MAX;
    Particle->PartInfo.Velocity.x = Velocity.x + (x - 0.5);
    Particle->PartInfo.Velocity.x /= 2.0;
    x = (float)rand()/(float)RAND_MAX;
    Particle->PartInfo.Velocity.y = Velocity.y + (x - 0.5);
    Particle->PartInfo.Velocity.y /= 2.0;
    x = (float)rand()/(float)RAND_MAX;
    Particle->PartInfo.Velocity.z = Velocity.z + (x - 0.5);
    Particle->PartInfo.Velocity.z /= 2.0;
    // Set the color
    Particle->PartInfo.Color[0] = 1.0;
    Particle->PartInfo.Color[1] = 1.0;
    Particle->PartInfo.Color[2] = 1.0;
    Particle->PartInfo.Color[3] = 1.0;
    // Create a random energy value between 0.5 and 1.0
    x = (float)rand()/(float)RAND_MAX;
    Particle->PartInfo.Energy = x + 0.5;
    if (Particle->PartInfo.Energy > 1.0)
        Particle->PartInfo.Energy = 1.0;
    // Set the size of the particle
    Particle->PartInfo.SizeX = 0.8;
    Particle->PartInfo.SizeY = 0.8;
}

void Spark::SetShape(PARTICLE* Particle)
{
    // Unused for now
}
//
//// Roman candle type
//void Roman::Update()
//{
//    VECTOR normal, pos, oldpos, temppos;
//    PARTICLE* tempParticle;
//
//    for (int loop = 1; loop <= SysInfo.numParticles; loop++)
//    {
//        tempParticle = ParticleList.Get(loop);
//        tempParticle->PartInfo.OldPos = tempParticle->PartInfo.Pos;
//        tempParticle->PartInfo.Velocity.x += 0.0;
//        tempParticle->PartInfo.Velocity.y += -0.003;
//        tempParticle->PartInfo.Velocity.z += 0.0;
//        if (tempParticle->PartInfo.Energy > 0.0)
//        {
//            tempParticle->PartInfo.Color[3] = tempParticle->PartInfo.Energy;
//            tempParticle->PartInfo.Energy -= 0.003;
//            oldpos = tempParticle->PartInfo.Pos;
//            pos = tempParticle->PartInfo.Pos + tempParticle->PartInfo.Velocity;
//            temppos = pos;
//            bool CollisionFlag = CheckForParticleCollision(tempParticle, oldpos, &temppos, &normal);
//
//            VECTOR VelocityVector;
//
//            if (CollisionFlag)
//            {
//                VECTOR vn = normal * (normal.dot(tempParticle->PartInfo.Velocity));
//                VECTOR vt = tempParticle->PartInfo.Velocity - vn;
//                VECTOR vel = (vt - vn);
//                tempParticle->PartInfo.Pos.x += vel.x;
//                tempParticle->PartInfo.Pos.y += vel.y;
//                tempParticle->PartInfo.Pos.z += vel.z;
//                tempParticle->PartInfo.Velocity.x = vel.x / 3.0;
//                tempParticle->PartInfo.Velocity.y = vel.y / 3.0;
//                tempParticle->PartInfo.Velocity.z = vel.z / 3.0;
//                tempParticle->PartInfo.Energy -= 0.3;
//            }
//            else
//            {
//                tempParticle->PartInfo.Pos.x += tempParticle->PartInfo.Velocity.x;
//                tempParticle->PartInfo.Pos.y += tempParticle->PartInfo.Velocity.y;
//                tempParticle->PartInfo.Pos.z += tempParticle->PartInfo.Velocity.z;
//            }
//        }
//        else
//        {
//            SetDefaults(tempParticle);
//        }
//
//        tempParticle->PartInfo.Color[0] += 0.006;
//        if (tempParticle->PartInfo.Color[0] > 1.0)
//            tempParticle->PartInfo.Color[0] = 1.0;
//        tempParticle->PartInfo.Color[1] += 0.006;
//        if (tempParticle->PartInfo.Color[1] > 1.0)
//            tempParticle->PartInfo.Color[1] = 1.0;
//        tempParticle->PartInfo.Color[2] += 0.006;
//        if (tempParticle->PartInfo.Color[2] > 1.0)
//            tempParticle->PartInfo.Color[2] = 1.0;
//    }
//}
//
//void Roman::Render(int nodeid)
//{
//    MATRIX mat;
//    VECTOR up;
//    VECTOR right;
//    PARTICLE* tempParticle;
//
//    glGetFloatv(GL_MODELVIEW_MATRIX, mat.Element);
//
//    glEnable(GL_BLEND);
//    glDepthMask(0);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//    glDisable(GL_LIGHTING);
//    glBindTexture(GL_TEXTURE_2D, SysInfo.TexID);
//
//    for (int loop = 1; loop <= SysInfo.numParticles; loop++)
//    {
//        tempParticle = ParticleList.Get(loop);
//
//        if (tempParticle->PartInfo.Leaf == nodeid)
//        {
//            if (tempParticle->PartInfo.Alive)
//            {
//                right.x = mat.Element[0];
//                right.y = mat.Element[4];
//                right.z = mat.Element[8];
//                right.Normalize();
//                right.x *= tempParticle->PartInfo.SizeX / 2;
//                right.y *= tempParticle->PartInfo.SizeX / 2;
//                right.z *= tempParticle->PartInfo.SizeX / 2;
//
//                up.x = mat.Element[1];
//                up.y = mat.Element[5];
//                up.z = mat.Element[9];
//                up.Normalize();
//                up.x *= tempParticle->PartInfo.SizeY / 2;
//                up.y *= tempParticle->PartInfo.SizeY / 2;
//                up.z *= tempParticle->PartInfo.SizeY / 2;
//
//                glColor4f(tempParticle->PartInfo.Color[0], tempParticle->PartInfo.Color[1], tempParticle->PartInfo.Color[2], tempParticle->PartInfo.Color[3]);
//                glBegin(GL_QUADS);
//                    glTexCoord2f(0.0f, 0.0f); glVertex3f(tempParticle->PartInfo.Pos.x + (-right.x - up.x), tempParticle->PartInfo.Pos.y + (-right.y - up.y), tempParticle->PartInfo.Pos.z + (-right.z - up.z));
//                    glTexCoord2f(1.0f, 0.0f); glVertex3f(tempParticle->PartInfo.Pos.x + (right.x - up.x), tempParticle->PartInfo.Pos.y + (right.y - up.y), tempParticle->PartInfo.Pos.z + (right.z - up.z));
//                    glTexCoord2f(1.0f, 1.0f); glVertex3f(tempParticle->PartInfo.Pos.x + (right.x + up.x), tempParticle->PartInfo.Pos.y + (right.y + up.y), tempParticle->PartInfo.Pos.z + (right.z + up.z));
//                    glTexCoord2f(0.0f, 1.0f); glVertex3f(tempParticle->PartInfo.Pos.x + (up.x - right.x), tempParticle->PartInfo.Pos.y + (up.y - right.y), tempParticle->PartInfo.Pos.z + (up.z - right.z));
//                glEnd();
//            }
//        }
//    }
//
//    glEnable(GL_LIGHTING);
//    glDepthMask(1);
//    glDisable(GL_BLEND);
//}
//
//void Roman::SetDefaults(PARTICLE* Particle)
//{
//    Particle->PartInfo.Alive = true;
//    Particle->PartInfo.Pos = SysInfo.Pos;
//    Particle->PartInfo.OldPos = SysInfo.Pos;
//    Particle->PartInfo.OrigPos = SysInfo.Pos;
//
//    VECTOR Velocity = SysInfo.Normal;
//
//    float x;
//    x = (float)rand()/(float)RAND_MAX;
//    Particle->PartInfo.Velocity.x = Velocity.x + (x - 0.5) / 5;
//    Particle->PartInfo.Velocity.x /= 4.0;
//    x = (float)rand()/(float)RAND_MAX;
//    Particle->PartInfo.Velocity.y = Velocity.y + (x - 0.5);
//    Particle->PartInfo.Velocity.y /= 4.0;
//    x = (float)rand()/(float)RAND_MAX;
//    Particle->PartInfo.Velocity.z = Velocity.z + (x - 0.5) / 5;
//    Particle->PartInfo.Velocity.z /= 4.0;
//
//    Particle->PartInfo.Color[0] = SysInfo.Color[0];
//    Particle->PartInfo.Color[1] = SysInfo.Color[1];
//    Particle->PartInfo.Color[2] = SysInfo.Color[2];
//    Particle->PartInfo.Color[3] = 1.0;
//
//    Particle->PartInfo.Energy = 1.0;
//    Particle->PartInfo.SizeX = 3.0;
//    Particle->PartInfo.SizeY = 3.0;
//}
//
//void Roman::SetShape(PARTICLE* Particle)
//{
//}
//
//// Bouncing type
//void Bouncy::Update()
//{
//    bool CollisionFlag;
//    VECTOR VelocityVector, normal, pos, oldpos, temppos;
//    PARTICLE* tempParticle;
//
//    for (int loop = 1; loop <= SysInfo.numParticles; loop++)
//    {
//        tempParticle = ParticleList.Get(loop);
//        tempParticle->PartInfo.OldPos = tempParticle->PartInfo.Pos;
//        oldpos = tempParticle->PartInfo.Pos;
//        pos = tempParticle->PartInfo.Pos + tempParticle->PartInfo.Velocity;
//        temppos = pos;
//        CollisionFlag = CheckForParticleCollision(tempParticle, oldpos, &temppos, &normal);
//
//        if (CollisionFlag)
//        {
//            VECTOR vn = normal * (normal.dot(tempParticle->PartInfo.Velocity));
//            VECTOR vt = tempParticle->PartInfo.Velocity - vn;
//            VECTOR vel = (vt - vn);
//            tempParticle->PartInfo.Pos.x += vel.x;
//            tempParticle->PartInfo.Pos.y += vel.y;
//            tempParticle->PartInfo.Pos.z += vel.z;
//            tempParticle->PartInfo.Velocity.x = vel.x;
//            tempParticle->PartInfo.Velocity.y = vel.y;
//            tempParticle->PartInfo.Velocity.z = vel.z;
//        }
//        else
//        {
//            tempParticle->PartInfo.Pos.x += tempParticle->PartInfo.Velocity.x;
//            tempParticle->PartInfo.Pos.y += tempParticle->PartInfo.Velocity.y;
//            tempParticle->PartInfo.Pos.z += tempParticle->PartInfo.Velocity.z;
//        }
//    }
//}
//
//void Bouncy::Render(int nodeid)
//{
//    MATRIX mat;
//    VECTOR up;
//    VECTOR right;
//    PARTICLE* tempParticle;
//
//    glGetFloatv(GL_MODELVIEW_MATRIX, mat.Element);
//
//    glEnable(GL_BLEND);
//    glDepthMask(0);
//    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
//    glDisable(GL_LIGHTING);
//    glBindTexture(GL_TEXTURE_2D, SysInfo.TexID);
//
//    for (int loop = 1; loop <= SysInfo.numParticles; loop++)
//    {
//        tempParticle = ParticleList.Get(loop);
//
//        if (tempParticle->PartInfo.Leaf == nodeid)
//        {
//            if (tempParticle->PartInfo.Alive)
//            {
//                right.x = mat.Element[0];
//                right.y = mat.Element[4];
//                right.z = mat.Element[8];
//                right.Normalize();
//                right.x *= tempParticle->PartInfo.SizeX / 2;
//                right.y *= tempParticle->PartInfo.SizeX / 2;
//                right.z *= tempParticle->PartInfo.SizeX / 2;
//
//                up.x = mat.Element[1];
//                up.y = mat.Element[5];
//                up.z = mat.Element[9];
//                up.Normalize();
//                up.x *= tempParticle->PartInfo.SizeY / 2;
//                up.y *= tempParticle->PartInfo.SizeY / 2;
//                up.z *= tempParticle->PartInfo.SizeY / 2;
//
//                glColor4f(0.0, 1.0, 0.0, 1.0);
//                glBegin(GL_QUADS);
//                    glTexCoord2f(0.0f, 0.0f); glVertex3f(tempParticle->PartInfo.Pos.x + (-right.x - up.x), tempParticle->PartInfo.Pos.y + (-right.y - up.y), tempParticle->PartInfo.Pos.z + (-right.z - up.z));
//                    glTexCoord2f(1.0f, 0.0f); glVertex3f(tempParticle->PartInfo.Pos.x + (right.x - up.x), tempParticle->PartInfo.Pos.y + (right.y - up.y), tempParticle->PartInfo.Pos.z + (right.z - up.z));
//                    glTexCoord2f(1.0f, 1.0f); glVertex3f(tempParticle->PartInfo.Pos.x + (right.x + up.x), tempParticle->PartInfo.Pos.y + (right.y + up.y), tempParticle->PartInfo.Pos.z + (right.z + up.z));
//                    glTexCoord2f(0.0f, 1.0f); glVertex3f(tempParticle->PartInfo.Pos.x + (up.x - right.x), tempParticle->PartInfo.Pos.y + (up.y - right.y), tempParticle->PartInfo.Pos.z + (up.z - right.z));
//                glEnd();
//            }
//        }
//    }
//
//    glEnable(GL_LIGHTING);
//    glDepthMask(1);
//    glDisable(GL_BLEND);
//}
//
//void Bouncy::SetDefaults(PARTICLE* Particle)
//{
//    Particle->PartInfo.Alive = true;
//    Particle->PartInfo.Pos = SysInfo.Pos;
//    Particle->PartInfo.OldPos = SysInfo.Pos;
//    Particle->PartInfo.OrigPos = SysInfo.Pos;
//
//    VECTOR Velocity = SysInfo.Normal;
//
//    float x;
//    x = (float)rand()/(float)RAND_MAX;
//    Particle->PartInfo.Velocity.x = Velocity.x + (x - 0.5) / 5;
//    Particle->PartInfo.Velocity.x /= 4.0;
//    x = (float)rand()/(float)RAND_MAX;
//    Particle->PartInfo.Velocity.y = Velocity.y + (x - 0.5);
//    Particle->PartInfo.Velocity.y /= 4.0;
//    x = (float)rand()/(float)RAND_MAX;
//    Particle->PartInfo.Velocity.z = Velocity.z + (x - 0.5) / 5;
//    Particle->PartInfo.Velocity.z /= 4.0;
//
//    Particle->PartInfo.Energy = 1.0;
//    Particle->PartInfo.SizeX = 3.0;
//    Particle->PartInfo.SizeY = 3.0;
//}
//
//void Bouncy::SetShape(PARTICLE* Particle)
//{
//}

void RenderParticles(int nodeid)
{
    //PManager.Render(nodeid);
}

ParticleSystem* CreateSparks(VECTOR OldPos, VECTOR Pos, VECTOR Normal, float radius)
{
    SystemInfo SI;
    ZeroMemory(&SI, sizeof(SI));
    SI.Visibility = true;
    SI.numParticles = rand()%8 + 10 * radius;
    SI.Type = spark;
    SI.TexID = texture[1].TexID;
    SI.Id = 1;
    SI.Pos = Pos;
    SI.Normal = Normal;
    SI.InitialVelocity = Pos - OldPos;

    ParticleSystem* sparky = new Spark;
    sparky->SysInfo = SI;

	return sparky;
    //PManager.Add(sparky);
}
