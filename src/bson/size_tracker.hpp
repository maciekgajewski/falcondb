#pragma once
#ifndef BSON_SIZE_TRACKER_HPP
#define BSON_SIZE_TRACKER_HPP

namespace mongo
{

/**
   used in conjuction with BSONObjBuilder, allows for proper buffer size to prevent crazy memory usage
 */
class BSONSizeTracker {
public:
    BSONSizeTracker() {
        _pos = 0;
        for ( int i=0; i<SIZE; i++ )
            _sizes[i] = 512; // this is the default, so just be consistent
    }

    ~BSONSizeTracker() {
    }

    void got( int size ) {
        _sizes[_pos++] = size;
        if ( _pos >= SIZE )
            _pos = 0;
    }

    /**
     * right now choosing largest size
     */
    int getSize() const {
        int x = 16; // sane min
        for ( int i=0; i<SIZE; i++ ) {
            if ( _sizes[i] > x )
                x = _sizes[i];
        }
        return x;
    }

private:
    enum { SIZE = 10 };
    int _pos;
    int _sizes[SIZE];
};

}

#endif // BSON_SIZE_TRACKER_HPP
