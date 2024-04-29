#include "../includes/next_queue.h"
#include <random>

NextQueue::NextQueue(){}

void NextQueue::init(Board* _board){
    board = _board;
    bag = createBag();
    next_bag = createBag();
}

std::queue<Tetramino> NextQueue::createBag(){
    std::queue<Tetramino> new_bag{};
    std::vector<Tetramino> pieces{
        createI(board),
        createJ(board), 
        createL(board), 
        createO(board), 
        createS(board), 
        createT(board), 
        createZ(board)
    };

    // shuffle pieces
    std::random_device rd;
    std::mt19937 g(rd());
    
    std::shuffle(pieces.begin(), pieces.end(), g);

    for (Tetramino& piece : pieces){
        new_bag.push(piece);
    }

    return new_bag;
}

Tetramino NextQueue::pop(){
    if (bag.empty()){
        bag = next_bag;
        next_bag = createBag();
    }

    Tetramino piece = bag.front();
    bag.pop();
    return piece;
}