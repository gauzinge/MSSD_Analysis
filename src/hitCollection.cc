//
//  hitCollection.cpp
//  
//
//  Created by Georg Auzinger on 17.12.12.
//
//

#include "hitCollection.h"

hitCollection::hitCollection() {
    file = NULL;
    tree = NULL;
}

hitCollection::~hitCollection (){
    if (file!=NULL){
        file->Close();
        delete file;
    }
//    file = NULL;
//    tree = NULL;
//    delete file;
//    delete tree;
}


unsigned int hitCollection::get_n_entries(){
    if (tree == NULL) {
        std::cerr << "[ERROR] No Trackhit Data available!" << std::endl;
        return 0;
    }
    return static_cast<unsigned int>(tree->GetEntries());
}
