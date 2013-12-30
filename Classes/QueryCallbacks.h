//  Author: Chris Campbell - www.iforce2d.net
//  -----------------------------------------
//
//  QueryCallbacks
//
//  Some callbacks used with QueryAABB to quickly find fixtures
//  in a rectangular region of the world.
//

#ifndef QUERYCALLBACKS_H
#define QUERYCALLBACKS_H

#include <Box2D/Box2D.h>

//
// TouchDownQueryCallback
//
// This is about the simplest type of query callback, for when
// we merely want to get one fixture - any fixture at all - that
// contains the touch point. There may actually be multiple fixtures
// that contain the touch point, and if so it's undetermined which
// one we will get.
//
class TouchDownQueryCallback : public b2QueryCallback
{
public:
    b2Vec2 m_point;             // this will be initialized with the touch point before calling QueryAABB
    b2Fixture* m_fixture;       // we will set this to the first fixture discovered that contains the touch point, and the program will check this after QueryAABB finishes

    // Constructor just records the touch point and initializes the fixture to NULL
    TouchDownQueryCallback(const b2Vec2& point)
    {
        m_point = point;
        m_fixture = NULL;
    }
    
    // This function will be called by Box2D for every fixture with an AABB that
    // overlaps the query region. We can check each of these to see if the touch
    // point is actually inside them or not. If it is, we record that fixture as
    // the result, and return false to tell Box2D this QueryAABB is finished.
    bool ReportFixture(b2Fixture* fixture)
    {
        // at this point we know the fixture ovelaps the AABB, but we also
        // check if it contains the touch point or not
        if ( fixture->TestPoint(m_point) ) {
            
            // this fixture contains the touch point, so we set this as the result and
            // return false to stop the QueryAABB from continuing any more
            m_fixture = fixture;
            return false;
        }
        
        // returning true keeps QueryAABB going to check other fixtures in the query region
        return true;
    }
    
};

#endif
