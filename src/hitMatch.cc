#include "hitMatch.h"
//#include "include/hitMatch.h"


const std::vector<int>& Event::GetHitLines() {
    return HitTreeRaw_;
}

std::string Event::Serialize() const {
    std::ostringstream outStringStream;
    // the format is: number of hits, hitLineNumber
    outStringStream << HitTreeRaw_.size();
    for (std::vector<int>::const_iterator it = HitTreeRaw_.begin();
         it != HitTreeRaw_.end(); ++it ) {
        outStringStream << " " << (*it);
    }
    return outStringStream.str();
}

void Event::addHit(const int& hitLine) {
    HitTreeRaw_.push_back(hitLine);
}

unsigned int Event::GetNHits() const {
    return HitTreeRaw_.size();
}




void EventCollection::addHit(const int& hitLineNumber, const int& eventNumber){
    Event& myEvent = myCollection_[eventNumber];
    myEvent.addHit( hitLineNumber );
}

const std::vector<int>& EventCollection::GetHitLines(int eventNumber) {
    Event& myEvent = myCollection_[eventNumber];
    return myEvent.GetHitLines();
}


void EventCollection::Serialize(std::ostream& outFileStream) {
    std::ostringstream outStringStream;
    std::map<int, Event>::const_iterator it;
    for (it=myCollection_.begin(); it!=myCollection_.end(); ++it) {
        const int& myEventNumber = it->first;
        const Event& myEvent = it->second;
        outStringStream.str("");
        outStringStream << myEventNumber << " ";
        outStringStream << myEvent.Serialize();
        outFileStream << outStringStream.str() << " -1 ";
    }
    outFileStream << std::endl;
}

void EventCollection::DeSerialize(std::ifstream& read) {
    int eventNumber;
    int nHits;
    int hitRow;
    int stopper;
    myCollection_.clear();
    while (!read.eof()) {
        read >> eventNumber;
        if (read.eof()) break;
        //    std::cout << "Event " << eventNumber << " ";
        if (eventNumber<0) {
            std::cerr << "[ERROR] negative event number found!" << std::endl;
            break;
        }
        read >> nHits;
        if (read.eof()) { std::cerr << "ERROR: unexpected enf of file" << std::endl; break; }
        //    std::cout << "has " << nHits << " hits: ";
        if (eventNumber<0) {
            std::cerr << "[ERROR] negative number of hits found!" << std::endl;
            break;
        }
        for (int iHit=0; iHit < nHits; ++iHit) {
            read >> hitRow;
            //      std::cout << hitRow << ", ";
            if (read.eof()) { std::cerr << "[ERROR] unexpected enf of file" << std::endl; break; }
            myCollection_[eventNumber].addHit(hitRow);
        }
        read >> stopper;
        //    std::cout << "that's all" << std::endl;;
        if (read.eof()) { std::cerr << "[ERROR] unexpected enf of file" << std::endl; break; }
        if (stopper!=-1) {
            std::cerr << "[ERROR] problem deserializing an EventCollection map: stopper not found when expected" << std::endl;
            break;
        }
    }
}








