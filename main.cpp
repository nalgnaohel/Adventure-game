#include "Utils.h"
#include "LTexture.h"
#include "Klee.h"
#include "Enemy.h"
#include "Buttons.h"
#include "Function.h"
#include "Timer.h"
#include "movingGround.h"
#include "Letter.h"

using namespace std;

const string WINDOW_TITLE = "Game ver 1.0";

bool initSDL();
bool loadMedia();
void close(Tile* tiles[]);
bool setTiles(Tile* tiles[], string mapName);
void setGhosts(string lv);
void clear(Tile* tiles[]);
void setupData();
void updateData();
void showData(); // debug only
int curLevelId = 0;

SDL_Window* gWindow = NULL;
LTexture gTBackground;
LTexture gTKlee[18];
LTexture gTInstr[INSTR_LENGTH];
LTexture gTKleeHb;
LTexture gTKleeHbBackground;
LTexture gTKleeExp;
LTexture gTKleeExpBg;
LTexture gTEnemy[TOTAL_ENEMIES][12];
LTexture gTArrow[6];
LTexture gTText;
LTexture gTMenuButtonsSpriteSheet[2];
LTexture gTEndLevelSign;
LTexture gTEndLevelDialog;
LTexture gTWinText;
LTexture gTLoseText;
LTexture gTFailText;
LTexture gTExpText;
LTexture gTPauseText[2];
LTexture gTTime;
LTexture gTypeTiles[TOTAL_TILE_TYPES];
LTexture gTEnemyHb;
LTexture gTEnemyHbBg;
LTexture gTHeart;
LTexture gTLevelDialog;
LTexture gTUlt;
LTexture gTUltCD;
LTexture gTUltTime;
LTexture gTLetter;
LTexture gTPreIns;
LTexture gTSmallDialog;
LTexture gTTitle;
Klee* klee = NULL;
SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;
TTF_Font* gTitleFont = NULL;
TTF_Font* gPauseFont1 = NULL;
TTF_Font* gPauseFont2 = NULL;
TTF_Font* gHalloFont = NULL;

Mix_Music* menuMusic = NULL;
Mix_Music* playMusic_normal = NULL;
Mix_Music* gameOverMusic = NULL;
Mix_Music* wonMusic = NULL;
Mix_Music* playMusic_fight = NULL;
Mix_Chunk* stab = NULL;
Mix_Chunk* page = NULL;
Mix_Chunk* level_up = NULL;
Mix_Chunk* fall = NULL;
Mix_Chunk* die = NULL;
Mix_Chunk* player_hurt = NULL;
Mix_Chunk* ghost_die[2] = {NULL, NULL};
Mix_Chunk* ghost_hurt[2] = {NULL, NULL};
Mix_Chunk* ghost_att = NULL;
Mix_Chunk* sound_e = NULL;
Mix_Chunk* sound_q = NULL;
Mix_Chunk* sound_s = NULL;
Mix_Chunk* button = NULL;

Letter* letter;
vector<string> clearedLevel;
vector<Buttons*> menuStart;
string lv;
vector<Enemy*> ghost[TOTAL_ENEMIES];
vector<pair<pair<int, int>, int>> ground;
vector<int> udId, lrId;
vector<movingGround*> upDown;
vector<movingGround*> leftRight;
vector<Buttons*> gameOverButtons;
vector<Buttons*> failedButtons;
vector<Buttons*> wonButtons;
vector<pair<pair<int, int>, pair<int, int>>> coor;
map<pair<pair<int, int>, int>, bool> is_mv;
Tile* tiles[MAX_TOTAL_TILES];
int TOTAL_TILES;
SDL_Rect gKleeSpriteClips[18][16];
int gKleeSpriteClipsSize[18];
SDL_Rect gKleeHbClips[2];
SDL_Rect gKleeExpbClips[2];
SDL_Rect gEnemyHbClips[2];
SDL_Rect gArrowSpriteClips[6][9];
SDL_Rect gPlayerHeart;
SDL_Rect gLetter;
int gArrowSpriteClipsSize[6];
SDL_Rect gEnemySpriteClips[2][12][7];
int gEnemySpriteClipsSize[2][12];
int tot_tiles = 0;
int st_level = 0;
pair<int, int> kFeet[9][16]; //x + fi, x + w - se

string fileKlee = "image/characters/Wanderer Magican";
string fileGhost[2] = {"Onre", "Yurei"};
string fileKleeState[9] = {"Attack_1", "Dead", "Hurt", "Idle", "Jump", "Magic_arrow", "Magic_sphere", "Run", "Walk"};
string fileArrow[2] = {"Charge_1", "Charge_2"};
string fileGhost0[6] = {"Attack_2", "Dead", "Hurt", "Run", "Scream", "Walk"};
string fileGhost1[6] = {"Attack_3", "Dead", "Hurt", "Run", "Scream", "Walk"};
string status[6] = {"Attack_2", "Dead", "Hurt", "Run", "Scream", "Walk"};
string levels[2] = {"1-1", "1-2"};

bool isGround(string typeTile){
    int id = stoi(typeTile);
    for(int j = 0; j < 9; j++){
        if(GROUND_ID[j] == id){
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]){
    if(!initSDL()){
        cout << "Unable to initialize\n";
    }
    else{
        if(!loadMedia()){
            cout << "Unable to load media\n";
        }
        else{
            bool isRunning = 1; bool play = 1;
            SDL_Color btmpColor = {121, 48, 48};

            Buttons* btmp = NULL; btmp = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            btmp->setPosition(SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 250);
            btmp->setButtonName("New game...", btmpColor, gFont);
            Buttons* btmp1 = NULL; btmp1 = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            btmp1->setPosition(btmp->getButtonBox().x, btmp->getButtonBox().y + MENU_BUTTON_HEIGHT + 25);
            btmp1->setButtonName("Load", btmpColor, gFont);
            Buttons* btmp2 = NULL; btmp2 = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            btmp2->setPosition(btmp1->getButtonBox().x, btmp1->getButtonBox().y + MENU_BUTTON_HEIGHT + 25);
            btmp2->setButtonName("Choose Level", btmpColor, gFont);
            Buttons* btmp3 = NULL; btmp3 = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            btmp3->setPosition(btmp2->getButtonBox().x, btmp2->getButtonBox().y + MENU_BUTTON_HEIGHT + 25);
            btmp3->setButtonName("Bye...", btmpColor, gFont);
            menuStart.pb(btmp); menuStart.pb(btmp1); menuStart.pb(btmp2); menuStart.pb(btmp3);

            Buttons* failButton = NULL;
            failButton = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            failButton->setPosition(SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 350);
            failButton->setButtonName("Give up!", btmpColor, gFont);
            failedButtons.pb(failButton);

            Buttons* wonButton1 = NULL;
            wonButton1 = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            wonButton1->setPosition(SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 350);
            wonButton1->setButtonName("Next level", btmpColor, gFont);
            wonButtons.pb(wonButton1);
            Buttons* wonButton2 = NULL;
            wonButton2 = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            wonButton2->setPosition(SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, wonButton1->getButtonBox().y + MENU_BUTTON_HEIGHT + 50);
            wonButton2->setButtonName("Leave...", btmpColor, gFont);
            wonButtons.pb(wonButton2);

            Buttons* goButton1 = NULL;
            goButton1 = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            goButton1->setPosition(SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, 350);
            goButton1->setButtonName("Try again!", btmpColor, gFont);
            gameOverButtons.pb(goButton1);

            Buttons* goButton2 = NULL;
            goButton2 = new Buttons(MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT);
            goButton2->setPosition(SCREEN_WIDTH / 2 - MENU_BUTTON_WIDTH / 2, goButton1->getButtonBox().y + MENU_BUTTON_HEIGHT + 50);
            goButton2->setButtonName("Leave...", btmpColor, gFont);
            gameOverButtons.pb(goButton2);
            //show menu
            bool new_p = 1; bool new_g = 1;
            string fname = "data.txt";
            fstream file(fname);
            vector<string> row;
            vector<vector<string>> content;
            string line, stringId;
            if(file.is_open()){
                while(getline(file, line)){
                    row.clear();
                    stringstream str(line);
                    while(getline(str, stringId, ' ')){
                        row.pb(stringId);
                    }
                    content.pb(row);
                }
                file.close();
            }
            if((int)content.size() > 1){
                for(int i = 0; i < stoi(content[1][0]); i++){
                    clearedLevel.pb(levels[i]);
                }
            }
            int gameState = showMenu(button, gTTitle, gHalloFont, clearedLevel, gFont, gTSmallDialog, gTPreIns,menuMusic, gTBackground, gTMenuButtonsSpriteSheet, gRenderer, menuStart);

            if(gameState == -1){
                play = 0; isRunning = 0;
            }
            else if(gameState == 1){
                new_p = 0; new_g = 0; curLevelId = stoi(content[1][1]); st_level = stoi(content[1][2]);
            }
            else{
                if(gameState == 0){
                    curLevelId = 0; clearedLevel.clear();
                }
                else{
                    curLevelId = gameState - 2;
                }
            }
            //run game
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);

            bool already = 0;
            while(play){
                SDL_Event event;
                bool gameOver = 0; bool won = 0;
                SDL_Rect camera = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
                bool died = 0;
                Timer fps_timer;
                if(!already){
                    lv = levels[curLevelId]; //cout << lv << '\n';
                    if(!setTiles(tiles, lv)){
                        play = 0; isRunning = 0;
                        cout << "Unable to load map!\n";
                    }
                    already = 1;
                }
                else{
                    klee = new Klee(gKleeSpriteClips[3][0]); //cout << "Fine\n";
                    if(!new_p){
                        klee->setState();
                        //showData();
                    }
                    else{
                        klee->updateState(); updateData(); //showData();
                    }
                    if(klee->ins == 0){
                        int xl = klee->getKleeBox().x + klee->getKleeBox().w + 50;
                        letter = new Letter(xl, gLetter);
                    }
                    if(!new_g){
                        //cout << coor.size() << ' ' << ground.size() << '\n';
                        setupData();// showData();
                        /*for(int i = 0; i < TOTAL_ENEMIES; i++){
                            for(int j = 0; j < (int)ghost[i].size(); j++){
                                cout << ghost[i][j]->getEnemyLeftX() << ' ' << ghost[i][j]->getEnemyRightX() << '\n';
                            }
                            cout << '\n';
                        }*/
                    }
                    else{
                        //cout << coor.size() << ' ' << ground.size() << '\n';
                        klee->updateState(); setGhosts(lv); updateData();
                        //showData();
                    }
                    klee->TOTAL_TILES = tot_tiles;
                    already = 1;
                    Mix_PlayMusic(playMusic_fight, -1); bool pause = 0; int pre_ins = -1;
                    while(isRunning){
                        if(!fps_timer.isStarted()){
                            fps_timer.start();
                        }
                        int st_fr = SDL_GetTicks();

                        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                        SDL_RenderClear(gRenderer);

                        gTBackground.render(0, 0, gRenderer);
                        for(int i = 0; i < tot_tiles; i++){
                            if(tiles[i]->getType() != -1){
                                tiles[i]->render(gRenderer, gTypeTiles, camera);
                            }
                        }
                        while(SDL_PollEvent(&event)){
                            if(event.type == SDL_QUIT){
                                isRunning = 0; play = 0; break;
                            }
                            if(event.type == SDL_KEYDOWN && event.key.repeat == 0){
                                if(event.key.keysym.sym == SDLK_x){
                                    if(!pause){
                                        Mix_PlayChannel(-1, page, 0);
                                        if(pre_ins == -1){
                                            pre_ins = klee->getKleeBox().x;
                                        }
                                        int gstate = showInstruction(page, gTBackground, gTEndLevelDialog, gTInstr, gRenderer);
                                        if(gstate == 1){
                                            isRunning = 0;
                                        }
                                        klee->update_ins(pre_ins);
                                        pre_ins = -1;
                                    }
                                }
                                if(event.key.keysym.sym == SDLK_p){
                                    pause = !pause;
                                    if(pause){
                                        fps_timer.pause();
                                        if(Mix_PausedMusic() == 0){
                                            Mix_PauseMusic();
                                        }
                                    }
                                    else{
                                        fps_timer.unpause();
                                        if(Mix_PausedMusic() == 1){
                                            Mix_ResumeMusic();
                                        }
                                    }

                                }
                            }
                            if(!pause){
                                klee->handleKleeEvent(event, gKleeSpriteClips, ground, kFeet);
                            }
                        }
                        //cout << tot_tiles << '\n';
                        if(pause){
                            SDL_Color pauseColor = {70, 12, 12};
                            if(!gTPauseText[0].loadFromRenderedText("PAUSE!", pauseColor, gRenderer, gPauseFont1)){
                                isRunning = 0; play = 0; already = 0; break;
                            }
                            else{
                                gTPauseText[0].render(SCREEN_WIDTH / 2 - gTPauseText[0].getWidth() / 2, SCREEN_HEIGHT / 2 - 60, gRenderer);
                            }
                            if(!gTPauseText[1].loadFromRenderedText("Press P to continue", pauseColor, gRenderer, gPauseFont2)){
                                isRunning = 0; play = 0; already = 0; break;
                            }
                            else{
                                gTPauseText[1].render(SCREEN_WIDTH / 2 - gTPauseText[1].getWidth() / 2, SCREEN_HEIGHT / 2 + 15, gRenderer);
                            }
                        }
                        if(!pause){
                            for(int i = 0; i < (int)upDown.size(); i++){
                                is_mv.erase(ground[udId[i]]);
                                upDown[i]->move(tiles, klee);
                                ground[udId[i]].fi.fi = upDown[i]->getLx();
                                ground[udId[i]].fi.se = upDown[i]->getRx();
                                ground[udId[i]].se = upDown[i]->getBox().y;
                                is_mv[ground[udId[i]]] = 1;
                                //cout << udId[i] << ":    " <<  ground[udId[i]].fi.fi << ' ' << ground[udId[i]].fi.se << ' ' << ground[udId[i]].se << '\n';
                            }
                            //cout <<  "----------------------" << '\n';
                            for(int i = 0; i < (int)leftRight.size(); i++){
                                is_mv.erase(ground[lrId[i]]);
                                leftRight[i]->move(tiles, klee);
                                ground[lrId[i]].fi.fi = leftRight[i]->getBox().x;
                                ground[lrId[i]].fi.se = leftRight[i]->getBox().x + leftRight[i]->getBox().w;
                                ground[lrId[i]].se = leftRight[i]->getBox().y;
                                is_mv[ground[lrId[i]]] = 1;
                            }
                            if(!klee->ins){
                                //if(letter != NULL){
                                //    cout << 2 << '\n';
                                //}
                                letter->move();
                                letter->render(gRenderer, gTLetter, &gLetter);
                                SDL_Rect kBox = klee->getKleeBox(); SDL_Rect lBox = letter->getBox();
                                if(checkCollision(kBox, lBox)){
                                    if(pre_ins == -1){
                                        pre_ins = kBox.x;
                                    }
                                    else{
                                        Mix_PlayChannel(-1, page, 0);
                                        int f = showInstruction(page, gTBackground, gTEndLevelDialog, gTInstr, gRenderer);
                                        if(f == 1){
                                            isRunning = 0; play = 0; break;
                                        }
                                        else{
                                            klee->ins = 1;
                                            klee->update_ins(pre_ins); klee->updateState(); updateData();
                                            pre_ins = -1;
                                        }
                                    }
                                }
                            }
                            klee->handleArrowList(sound_q, sound_s, ghost_die, gRenderer, gTArrow, gArrowSpriteClips, gArrowSpriteClipsSize, camera, ghost, gEnemySpriteClips);
                            klee->move(curLevelId, is_mv, tiles, ground, gKleeSpriteClips, kFeet);
                            klee->setCamera(curLevelId, camera);
                            if(!died){
                                for(int i = 0; i < TOTAL_ENEMIES; i++){
                                    int j = 0;
                                    while(j < (int)ghost[i].size()){
                                        SDL_Rect kBox = klee->getKleeBox();
                                        ghost[i][j]->action(playMusic_fight, stab, ghost_att, kBox, gEnemySpriteClips, gEnemySpriteClipsSize);
                                        klee->updateKleeHealth(player_hurt, ghost[i][j]->getEnemyDamage(), gKleeSpriteClips);
                                        //if(ghost[i][j]->getEnemyDamage() > 0){
                                            //cout << "Hurt:   " << klee->getKleeHealth() << '\n';
                                        //}
                                        //cout << "Klee Health: " << klee->getKleeHealth() << '\n';
                                        if(klee->getKleeHealth() <= 0){
                                            died = 1; klee->spriteId = 1; klee->kleeFrame = 0;
                                            SDL_Rect kBox = klee->getKleeBox();
                                            klee->updateKleeBox(kBox, ground, kFeet);
                                            Mix_PlayChannel(-1, die, 0);
                                            break;
                                        }
                                        if(klee->spriteId % 9 == 0 && klee->kleeFrame == 4 * KLEE_ID_FRAME[klee->spriteId % 9]){
                                            Mix_PlayChannel(-1, sound_e, 0);
                                            SDL_Rect dam;
                                            if(klee->spriteId == 0){
                                                dam = {klee->getKleeBox().x, klee->getKleeBox().y - 36, klee->getKleeBox().w + 36, klee->getKleeBox().h + 72};
                                            }
                                            else{
                                                dam = {klee->getKleeBox().x - 36, klee->getKleeBox().y - 36, klee->getKleeBox().w + 36, klee->getKleeBox().h + 72};
                                            }
                                            SDL_Rect eBox = ghost[i][j]->getEnemyBox();
                                            if(checkCollision(dam, eBox)){
                                                //cout << "Ghost" << i << ' ' << j << " - " << "Health: ";
                                                ghost[i][j]->updateHealth(ghost_die, 8, gEnemySpriteClips);
                                                //cout << ghost[i][j]->getHealth() << '\n';
                                            }
                                        }
                                        int fr = ghost[i][j]->frame;
                                        if(ghost[i][j]->spriteId % 6 == 1 && fr == gEnemySpriteClipsSize[ghost[i][j]->type][1] * GHOST_ID_FRAME[ghost[i][j]->type][1] - 1){
                                            //delete this ghost
                                            klee->updateExp(level_up, ghost[i][j]->getExp_val());
                                            Enemy* e = ghost[i].at(j);
                                            ghost[i].erase(ghost[i].begin() + j);
                                            delete e;
                                            e = NULL;
                                            j--;
                                        }
                                        j++;
                                    }
                                    if(died){
                                        break;
                                    }
                                }
                            }
                            //cout << klee->onGround(ground) << ' ' << klee->spriteId << ' ' << klee->kleeFrame << ' ' << klee->getKleeHealth() << ' ';
                            if((died && klee->kleeFrame == gKleeSpriteClipsSize[1] * KLEE_ID_FRAME[1] - 1)){
                                gameOver = true;
                                klee->life--;
                            }
                            if(klee->getKleeBox().y > SCREEN_HEIGHT){
                                Mix_PlayChannel(-1, fall, 0);
                                gameOver = true; klee->life--;
                            }
                        }
                        if(gameOver){
                            isRunning = false;
                            st_level = st_level + fps_timer.getCurTicks();
                            fps_timer.stop();
                        }
                        if(klee->getKleeBox().x + klee->getKleeBox().w >= LEVEL_WIDTH[curLevelId]){
                            won = true; isRunning = false;
                            st_level = st_level + fps_timer.getCurTicks();
                            fps_timer.stop();
                        }
                            //cout << klee->spriteId << ' ' << klee->kleeFrame << ' ' << klee->onGround(ground) << '\n';
                        SDL_Rect* currentClip;
                        currentClip = &gKleeSpriteClips[klee->spriteId][klee->kleeFrame / KLEE_ID_FRAME[klee->spriteId % 9]];
                        klee->render(gTKlee[klee->spriteId], gTKleeHb, gTKleeHbBackground, gKleeHbClips, gTKleeExp, gTKleeExpBg, gKleeExpbClips, gRenderer, currentClip, camera);

                        for(int i = 0; i < TOTAL_ENEMIES; i++){
                            for(int j = 0; j < (int)ghost[i].size(); j++){
                                int fr = ghost[i][j]->frame; int sp = ghost[i][j]->spriteId;
                                SDL_Rect* clip = &gEnemySpriteClips[i][sp][fr / GHOST_ID_FRAME[i][sp % 6]];
                                ghost[i][j]->render(gTEnemyHb, gTEnemyHbBg, gEnemyHbClips, gTEnemy[i][sp], gRenderer, clip, camera);
                            }
                        }

                        int y_signBox = LEVEL_HEIGHT - gTEndLevelSign.getHeight() - 3 * TILE_HEIGHT;
                        if(curLevelId == 1){
                            y_signBox -= TILE_HEIGHT;
                        }
                        SDL_Rect signBox = {LEVEL_WIDTH[curLevelId] - gTEndLevelSign.getWidth(), y_signBox, gTEndLevelSign.getWidth(), gTEndLevelSign.getHeight()};
                        if(checkCollision(camera, signBox)){
                            gTEndLevelSign.render(LEVEL_WIDTH[curLevelId] - gTEndLevelSign.getWidth() - camera.x - 10, y_signBox - camera.y, gRenderer);
                        }
                        int now_time = SDL_GetTicks() - st_fr;
                        int elasped = fps_timer.getCurTicks();
                        int real_time = (elasped + st_level) / 1000;
                        string time_ = "Time: " + to_string(real_time);
                        SDL_Color titleColor = {255, 255, 255};
                        if(!gTTime.loadFromRenderedText(time_, titleColor, gRenderer, gFont)){
                            cout << "Unable to display time\n"; break;
                        }
                        else{
                            gTTime.render(10, 10, gRenderer);
                        }
                        if(klee->s_cd){
                            int scd = klee->s_cd / FRAME_PER_SECOND; string cd = to_string(scd);
                            if(!gTUltTime.loadFromRenderedText(cd, titleColor, gRenderer, gFont)){
                                break;
                            }
                            else{
                                gTUltTime.render(10, 40 + gTTime.getHeight(), gRenderer);
                                gTUlt.render(gTUltTime.getWidth() + 40, 40 + gTTime.getHeight(), gRenderer);
                                gTUltCD.render(gTUltTime.getWidth() + 40, 40 + gTTime.getHeight(), gRenderer);
                            }
                        }
                        string extp_text = "Exp: ";
                        if(!gTExpText.loadFromRenderedText(extp_text, titleColor, gRenderer, gFont)){
                            break;
                        }
                        else{
                            gTExpText.render(SCREEN_WIDTH / 2, 10, gRenderer);
                        }
                        string heart_text = "Life: " + to_string(klee->life);
                        if(!gTExpText.loadFromRenderedText(heart_text, titleColor, gRenderer, gFont)){
                            break;
                        }
                        else{
                            gTExpText.render(SCREEN_WIDTH / 2, 30 + gTExpText.getHeight(), gRenderer);
                        }

                        int frameTime = 1000 / FRAME_PER_SECOND;
                        if(now_time < frameTime){
                            int dl = frameTime - now_time;
                            SDL_Delay(dl);
                        }
                        SDL_RenderPresent(gRenderer);

                        if(!pause){
                            klee->updateFrame(gKleeSpriteClips, gKleeSpriteClipsSize, ground, gArrowSpriteClips, camera, kFeet);
                            SDL_Rect kBox = klee->getKleeBox();
                            for(int i = 0; i < TOTAL_ENEMIES; i++){
                                for(int j = 0; j < (int)ghost[i].size(); j++){
                                    ghost[i][j]->updateFrame(playMusic_fight, stab, ghost_att, kBox, gEnemySpriteClips, gEnemySpriteClipsSize);
                                }
                            }
                            klee->s_cd = max(0, klee->s_cd - 1);
                        }
                    }
                    if(gameOver){
                        string title = "Game Over!";
                        if(klee->life == 0){
                            title = "You failed!";
                            //klee->updateState(); updateData(); showData();
                            int gState = showFail(button, gameOverMusic, title, gTBackground, gTEndLevelDialog, gTFailText, gTMenuButtonsSpriteSheet, gRenderer, gFont, failedButtons);
                            if(gState == 0){
                                SDL_RenderClear(gRenderer);
                                int nxtState = showMenu(button, gTTitle, gHalloFont, clearedLevel, gFont, gTSmallDialog, gTPreIns, menuMusic, gTBackground, gTMenuButtonsSpriteSheet, gRenderer, menuStart);
                                if(nxtState == -1){
                                    isRunning = 0; play = 0; break;
                                }
                                else if(nxtState == 1){
                                    isRunning = 1; gameOver = 0; already = 0; new_p = 0; new_g = 0;
                                    clear(tiles);
                                }
                                else{
                                    isRunning = 1; klee->life = 3; already = 0;gameOver = 0; st_level = 0;
                                    if(nxtState == 0){
                                        curLevelId = nxtState; new_p = 1; new_g = 1; clearedLevel.clear();
                                    }
                                    else{
                                        curLevelId = nxtState - 2; new_p = 1; new_g = 1;
                                    }
                                    clear(tiles);
                                }
                            }
                            else{
                                isRunning = 0; play = 0; break;
                            }
                        }
                        else{
                            klee->updateState(); updateData(); //showData();
                             cout << st_level << '\n';
                            int gState = showGameOver(button, gameOverMusic, title, gTBackground, gTEndLevelDialog, gTLoseText, gTMenuButtonsSpriteSheet, gRenderer, gFont, gameOverButtons);
                            if(gState == 1){ // backto menu
                                SDL_RenderClear(gRenderer);
                                int nxt_state = showMenu(button, gTTitle, gHalloFont, clearedLevel, gFont, gTSmallDialog, gTPreIns, menuMusic, gTBackground, gTMenuButtonsSpriteSheet, gRenderer, menuStart);
                                if(nxt_state == -1){
                                    isRunning = 0; play = 0; break;
                                }
                                else if(nxt_state == 1){
                                    isRunning = 1; already = 0; gameOver = 0; new_p = 0; new_g = 0;
                                    clear(tiles);
                                }
                                else{
                                    isRunning = 1; klee->life = 3; already = 0;gameOver = 0; st_level = 0;
                                    if(nxt_state == 0){
                                        curLevelId = 0; new_p = 1; new_g = 1; clearedLevel.clear();
                                    }
                                    else{
                                        curLevelId = nxt_state -2; new_p = 1; new_g = 1;
                                    }
                                    clear(tiles);
                                }
                            }
                            else{
                                isRunning = 1; gameOver = 0; new_p = 0; new_g = 0; already = 0;
                                clear(tiles);
                            }
                        }
                        if(klee != NULL){
                            delete klee; klee = NULL;
                        }
                    }
                    if(won){
                        string title = "You won!";
                        bool ck = 0; cout << levels[curLevelId] << "!\n";
                        for(int i =0; i <(int)clearedLevel.size(); i++){
                            //cout << clearedLevel[i] << ' ';
                            if(clearedLevel[i] == levels[curLevelId]){
                                ck = 1; //cout << clearedLevel[i] << '\n';
                                break;
                            }
                        }
                        //cout << '\n';
                        if(!ck){
                            cout << "F\n";
                            clearedLevel.pb(levels[curLevelId]);
                        }
                        int gState = showClearLevel(button, clearedLevel, wonMusic, title, gTBackground, gTEndLevelDialog, gTWinText, gFont, gTMenuButtonsSpriteSheet, gRenderer, wonButtons);
                        if(gState == -1){
                            play = 0; isRunning = 0; break;
                        }
                        else if(gState == 0 && curLevelId < 1) {
                            curLevelId++;
                            isRunning = 1; won = 0; already = 0;
                            new_p = 0; new_g = 1;
                            klee->resetcoor();
                            klee->updateState(); updateData();
                            //cout << "Win:\n";
                            //showData();
                            clear(tiles);
                        }
                        else if(gState == 0 && curLevelId == 1){
                            isRunning = 1; won = 0; already = 0;
                            new_p = 0; new_g = 0;
                            klee->resetcoor(); klee->updateState(); updateData();
                            clear(tiles);
                        }
                        else{
                            klee->updateState(); updateData(); showData();
                            int nxt_state = showMenu(button, gTTitle, gHalloFont, clearedLevel, gFont, gTSmallDialog, gTPreIns, menuMusic, gTBackground, gTMenuButtonsSpriteSheet, gRenderer, menuStart);
                            if(nxt_state == -1){
                                isRunning = 0; play = 0; break;
                            }
                            else if(nxt_state == 1){
                                isRunning = 1; won = 0; already = 0; new_p = 0; new_g = 0;
                                clear(tiles);
                            }
                            else{
                                isRunning = 1; won = 0;klee->life = 3; already = 0; st_level = 0;
                                if(nxt_state == 0){
                                    curLevelId = 0; clearedLevel.clear();
                                }
                                else{
                                    curLevelId = nxt_state - 2;
                                }
                                new_p = 1; new_g = 1;
                                clear(tiles);
                            }
                        }
                        if(klee != NULL){
                            delete klee; klee = NULL;
                        }
                    }
                }
            }
        }
        close(tiles);
        //Destroy window
        SDL_DestroyRenderer(gRenderer);
        SDL_DestroyWindow(gWindow);
        gRenderer = NULL; gWindow = NULL; gFont = NULL;

        //Quit SDL subsystems
        TTF_Quit();
        IMG_Quit();
        Mix_Quit();
        SDL_Quit();
    }
    return 0;
}

bool initSDL(){
    bool success = true;
    if(SDL_Init(SDL_INIT_EVERYTHING) == -1){
        cout << "Unable to initialize\n"; success = false;
    }
    else{
        if(!SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" )){
			printf( "Warning: Linear texture filtering not enabled!" );
		}

        //Create window
        gWindow = SDL_CreateWindow(WINDOW_TITLE.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if(gWindow == NULL){
            printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
            success = false;
        }
        else{
            //Get window surface
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
            if(gRenderer == NULL){
                cout << "Could not create renderer\n"; success = false;
            }
            else{
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
                //init IMG loading
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) && imgFlags)){
                    cout << "Could not initialize SDL_Image! SDL_image Error: " << IMG_GetError() << '\n';
                    success = false;
                }

                if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 ){
                    cout << "SDL_mixer could not initialize! SDL_mixer Error: " << Mix_GetError() << '\n';
                    success = false;
                }
                //init TTF
                if(TTF_Init() == -1){
                    cout << "Could not initialize SDL_TTF! SDL_ttf Error: " << TTF_GetError() << '\n';
                    success = false;
                }


            }
        }
    }
    return success;
}

bool loadMedia(){
    bool success = true;
    if(!gTBackground.loadFromFile("image/png/BG/background_2.jpg", gRenderer)){
        cout << "Failed to load background image!"; success = false;
    }
    if(!gTEndLevelSign.loadFromFile("image/png/Object/Sign_1.png", gRenderer)){
        cout << "Failed to load sign!"; success = false;
    }
    if(!gTSmallDialog.loadFromFile("image/paper/smaller-dialog.png", gRenderer)){
        success = false;
    }
    for(int i = 0; i < 18; i++){
        string tmp;
        if(i >= 9){
            tmp = fileKleeState[i - 9] + "_f";
        }
        else{
            tmp = fileKleeState[i];
        }
        string fname = fileKlee + "/" + tmp + ".png";
        //cout << i << ": " << fname << '\n';
        if(!gTKlee[i].loadFromFile(fname, gRenderer)){
            cout << "Failed to load character's " << fileKleeState[i] << " state\n"; success = false;
        }
        //cout << '\n';
    }
    if(success){
        gKleeSpriteClips[0][0] = {31, 64, 41, 64}; gKleeSpriteClips[9][0] = {824, 64, 41, 64}; kFeet[0][0] = mp(2, 5);
        gKleeSpriteClips[0][1] = {160, 65, 38, 63}; gKleeSpriteClips[9][1] = {698, 65, 38, 63}; kFeet[0][1] = mp(1, 3);
        gKleeSpriteClips[0][2] = {282, 62, 68, 66}; gKleeSpriteClips[9][2] = {545, 62, 68, 66}; kFeet[0][2] = mp(7, 27);
        gKleeSpriteClips[0][3] = {405, 45, 84, 83}; gKleeSpriteClips[9][3] = {406, 45, 84, 83}; kFeet[0][3] = mp(12, 38);
        gKleeSpriteClips[0][4] = {532, 41, 90, 87}; gKleeSpriteClips[9][4] = {274, 41, 90, 87}; kFeet[0][4] = mp(13, 43);
        gKleeSpriteClips[0][5] = {661, 29, 99, 99}; gKleeSpriteClips[9][5] = {136, 29, 99, 99}; kFeet[0][5] = mp(12, 53);
        gKleeSpriteClips[0][6] = {795, 62, 72, 66}; gKleeSpriteClips[9][6] = {31, 62, 72, 66}; kFeet[0][6] = mp(6, 32);
        gKleeSpriteClipsSize[0] = 7; gKleeSpriteClipsSize[9] = 7;

        gKleeSpriteClips[2][0] = {41, 69, 41, 59}; gKleeSpriteClips[11][0] = {429, 69, 42, 59}; kFeet[2][0] = mp(0, 9);
        gKleeSpriteClips[2][1] = {166, 69, 40, 59}; gKleeSpriteClips[11][1] = {303, 69, 40, 59}; kFeet[2][1] = mp(0, 8);
        gKleeSpriteClips[2][2] = {295, 69, 39, 59}; gKleeSpriteClips[11][2] = {177, 69, 39, 59}; kFeet[2][2] = mp(2, 7);
        gKleeSpriteClips[2][3] = {425, 69, 42, 59}; gKleeSpriteClips[11][3] = {44, 69, 42, 59}; kFeet[2][3] = mp(0, 10);
        gKleeSpriteClipsSize[2] = 4; gKleeSpriteClipsSize[11] = 4;

        gKleeSpriteClips[1][0] = {32, 69, 39, 59}; gKleeSpriteClips[10][0] = {440, 69, 39, 59};
        gKleeSpriteClips[1][1] = {159, 72, 40, 56}; gKleeSpriteClips[10][1] = {314, 72, 40, 56};
        gKleeSpriteClips[1][2] = {289, 88, 58, 40};  gKleeSpriteClips[10][2] = {166, 88, 58, 40};
        gKleeSpriteClips[1][3] = {416, 118, 62, 10}; gKleeSpriteClips[10][3] = {34, 118, 62, 10};
        gKleeSpriteClipsSize[1] = 4; gKleeSpriteClipsSize[10] = 4;

        gKleeSpriteClips[3][0] = {46, 62, 35, 66}; gKleeSpriteClips[12][0] = {943, 62, 35, 66};
        gKleeSpriteClipsSize[3] = 1; gKleeSpriteClipsSize[12] = 1;

        gKleeSpriteClips[4][0] = {19, 68, 59, 60}; gKleeSpriteClips[13][0] = {945, 68, 59, 60}; kFeet[4][0] = mp(20, 6);
        gKleeSpriteClips[4][1] = {166, 72, 35, 56}; gKleeSpriteClips[13][1] = {823, 72, 35, 56};
        gKleeSpriteClips[4][2] = {292, 74, 39, 54}; gKleeSpriteClips[13][2] = {690, 74, 39, 54};
        gKleeSpriteClips[4][3] = {428, 57, 41, 63}; gKleeSpriteClips[13][3] = {555, 57, 41, 63}; kFeet[4][3] = mp(3, 12);
        gKleeSpriteClips[4][4] = {550, 45, 49, 63}; gKleeSpriteClips[13][4] = {425, 45, 49, 63};
        gKleeSpriteClips[4][5] = {678, 53, 67, 72}; gKleeSpriteClips[13][5] = {280, 53, 67, 72}; kFeet[4][5] = mp(33, 25);
        gKleeSpriteClips[4][6] = {798, 71, 60, 57}; gKleeSpriteClips[13][6] = {167, 71, 60, 57}; kFeet[4][6] = mp(19, 12);
        gKleeSpriteClips[4][7] = {940, 72, 34, 56}; gKleeSpriteClips[13][7] = {51, 72, 34, 56};
        gKleeSpriteClipsSize[4] = 8; gKleeSpriteClipsSize[13] = 8;

        gKleeSpriteClips[5][0] = {37, 65, 39, 63}; gKleeSpriteClips[14][0] = {692, 65, 39, 63}; kFeet[5][0] = mp(1, 4);
        gKleeSpriteClips[5][1] = {163, 65, 47, 63}; gKleeSpriteClips[14][1] = {558, 65, 47, 63}; kFeet[5][1] = mp(3, 10);
        gKleeSpriteClips[5][2] = {293, 63, 55, 65}; gKleeSpriteClips[14][2] = {420, 63, 55, 65}; kFeet[5][2] = mp(1, 19);
        gKleeSpriteClips[5][3] = {420, 63, 55, 65}; gKleeSpriteClips[14][3] = {292, 63, 55, 65}; kFeet[5][3] = mp(2, 19);
        gKleeSpriteClips[5][4] = {550, 65, 54, 63}; gKleeSpriteClips[14][4] = {163, 63, 54, 65}; kFeet[5][4] = mp(2, 19);
        gKleeSpriteClips[5][5] = {678, 66, 54, 62}; gKleeSpriteClips[14][5] = {37, 63, 54, 65}; kFeet[5][5] = mp(0, 19);
        gKleeSpriteClipsSize[5] = 6; gKleeSpriteClipsSize[14] = 6;

        gKleeSpriteClips[6][0] = {32, 65, 40, 63}; gKleeSpriteClips[15][0] = {1976, 65, 40, 63}; kFeet[6][0] = mp(6, 0);
        gKleeSpriteClips[6][1] = {163, 65, 44, 63}; gKleeSpriteClips[15][1] = {1841, 65, 44, 63}; kFeet[6][1] = mp(3, 7);
        gKleeSpriteClips[6][2] = {293, 63, 60, 65}; gKleeSpriteClips[15][2] = {1695, 63, 60, 65}; kFeet[6][2] = mp(1, 25);
        gKleeSpriteClips[6][3] = {422, 63, 60, 65}; gKleeSpriteClips[15][3] = {1567, 63, 60, 65}; kFeet[6][3] = mp(0, 25);
        gKleeSpriteClips[6][4] = {549, 63, 61, 65}; gKleeSpriteClips[15][4] = {1438, 63, 61, 65}; kFeet[6][4] = mp(1, 26);
        gKleeSpriteClips[6][5] = {677, 63, 62, 65}; gKleeSpriteClips[15][5] = {1309, 63, 62, 65}; kFeet[6][5] = mp(1, 27);
        gKleeSpriteClips[6][6] = {805, 63, 63, 65}; gKleeSpriteClips[15][6] = {1180, 63, 63, 65}; kFeet[6][6] = mp(1, 27);
        gKleeSpriteClips[6][7] = {933, 63, 58, 65}; gKleeSpriteClips[15][7] = {1064, 63, 58, 65}; kFeet[6][7] = mp(1, 23);
        gKleeSpriteClips[6][8] = {1061, 63, 58, 65}; gKleeSpriteClips[15][8] = {929, 63, 58, 65}; kFeet[6][8] = mp(1, 23);
        gKleeSpriteClips[6][9] = {1189, 63, 60, 65}; gKleeSpriteClips[15][9] = {799, 63, 60, 65}; kFeet[6][9] = mp(1, 24);
        gKleeSpriteClips[6][10] = {1317, 63, 61, 65}; gKleeSpriteClips[15][10] = {670, 63, 61, 65}; kFeet[6][10] = mp(1, 26);
        gKleeSpriteClips[6][11] = {1446, 64, 60, 64}; gKleeSpriteClips[15][11] = {542, 64, 60, 64}; kFeet[6][11] = mp(0, 26);
        gKleeSpriteClips[6][12] = {1564, 64, 75, 64}; gKleeSpriteClips[15][12] = {409, 64, 75, 64}; kFeet[6][12] = mp(10, 30);
        gKleeSpriteClips[6][13] = {1688, 64, 67, 64}; gKleeSpriteClips[15][13] = {293, 64, 67, 64}; kFeet[6][13] = mp(13, 19);
        gKleeSpriteClips[6][14] = {1816, 64, 67, 64}; gKleeSpriteClips[15][14] = {165, 64, 67, 64}; kFeet[6][14] = mp(13, 19);
        gKleeSpriteClips[6][15] = {1954, 64, 57, 64}; gKleeSpriteClips[15][15] = {37, 64, 57, 64}; kFeet[6][15] = mp(4, 19);
        gKleeSpriteClipsSize[6] = 16; gKleeSpriteClipsSize[15] = 16;

        gKleeSpriteClips[7][0] = {23, 67, 44, 61}; gKleeSpriteClips[16][0] = {957, 67, 44, 61};
        gKleeSpriteClips[7][1] = {143, 66, 54, 62}; gKleeSpriteClips[16][1] = {827, 66, 54, 62};
        gKleeSpriteClips[7][2] = {265, 68, 64, 60}; gKleeSpriteClips[16][2] = {695, 68, 64, 60};
        gKleeSpriteClips[7][3] = {389, 65, 65, 63}; gKleeSpriteClips[16][3] = {570, 65, 65, 63};
        gKleeSpriteClips[7][4] = {530, 65, 50, 63}; gKleeSpriteClips[16][4] = {444, 65, 50, 63};
        gKleeSpriteClips[7][5] = {669, 66, 37, 62}; gKleeSpriteClips[16][5] = {319, 66, 37, 62};
        gKleeSpriteClips[7][6] = {796, 68, 45, 60}; gKleeSpriteClips[16][6] = {184, 68, 45, 60};
        gKleeSpriteClips[7][7] = {919, 67, 43, 61}; gKleeSpriteClips[16][7] = {63, 67, 43, 61};
        gKleeSpriteClipsSize[7] = 8; gKleeSpriteClipsSize[16] = 8;

        gKleeSpriteClips[8][0] = {49, 62, 31, 66}; gKleeSpriteClips[17][0] = {816, 62, 31, 66}; kFeet[8][0] = mp(5, 11);
        gKleeSpriteClips[8][1] = {172, 62, 46, 66}; gKleeSpriteClips[17][1] = {678, 62, 46, 66}; kFeet[8][1] = mp(7, 16);
        gKleeSpriteClips[8][2] = {289, 62, 43, 66}; gKleeSpriteClips[17][2] = {544, 62, 43, 66}; kFeet[8][2] = mp(8, 8);
        gKleeSpriteClips[8][3] = {422, 62, 42, 66}; gKleeSpriteClips[17][3] = {432, 62, 42, 66}; kFeet[8][3] = mp(13, 13);
        gKleeSpriteClips[8][4] = {554, 62, 35, 66}; gKleeSpriteClips[17][4] = {307, 62, 35, 66}; kFeet[8][4] = mp(12, 11);
        gKleeSpriteClips[8][5] = {688, 62, 26, 66}; gKleeSpriteClips[17][5] = {182, 62, 26, 66}; kFeet[8][15] = mp(4, 0);
        gKleeSpriteClips[8][6] = {817, 62, 29, 66}; gKleeSpriteClips[17][6] = {50, 62, 29, 66}; kFeet[8][16] = mp(4, 3);
        gKleeSpriteClipsSize[8] = 7; gKleeSpriteClipsSize[17] = 7;
    }
    if(!gTKleeHbBackground.loadFromFile("image/characters/Healthbar/Healthbar DARK.png", gRenderer)){
        cout << "Failed to render healthbar background!\n"; success = false;
    }
    if(!gTKleeHb.loadFromFile("image/characters/Healthbar/Healthbar DARK.png", gRenderer)){
        cout << "Failed to render healthbar!\n"; success = false;
    }
    if(!gTKleeExpBg.loadFromFile("image/characters/Healthbar/expbar.png", gRenderer)){
        cout << "Failed to render expbar background!\n"; success = false;
    }
    if(!gTKleeExp.loadFromFile("image/characters/Healthbar/expbar.png", gRenderer)){
        cout << "Failed to render expbar!\n"; success = false;
    }
    if(!gTEnemyHbBg.loadFromFile("image/characters/Healthbar/enemybar.png", gRenderer)){
        cout << "Failed to render ghost's healthbar\n";
        success = false;
    }
    //cout << 5 << '\n';
    if(!gTEnemyHb.loadFromFile("image/characters/Healthbar/enemybar.png", gRenderer)){
        success = false;
    }
    if(success){
        gKleeHbClips[0] = {9, 94, 76, 18}; //background klee health
        gKleeHbClips[1] = {28, 90, 56, 4};
        gKleeExpbClips[0] = {18, 18, 262, 30}; //background exp
        gKleeExpbClips[1] = {18, 52, 262, 30};
        gEnemyHbClips[0] = {40, 72, 66, 7};
        gEnemyHbClips[1] = {40, 100, 66, 7};
    }
    if(!gTUlt.loadFromFile("image/characters/Wanderer Magican/Charge_1_cd.png", gRenderer)){
        success = false;
    }
    if(!gTUltCD.loadFromFile("image/characters/Wanderer Magican/no_s.png", gRenderer)){
        success = false;
    }
    for(int i = 0; i < 6; i++){
        string tmp;
        if(i == 2 || i == 5){
            continue;
        }
        if(i < 2){
            tmp = fileArrow[i];
        }
        else{
            tmp = fileArrow[i - 3] + "_f";
        }
        //cout << "image/characters/Wanderer Magican";// << '/' << tmp << ".png" << '\n';
        string fname = "image/characters/Wanderer Magican/" + tmp + ".png";
        //cout << i << ": " << fname << '\t';
        if(!gTArrow[i].loadFromFile(fname, gRenderer)){
            cout << "Failed to load arrow!\n"; success = false;
        }
    }
    if(success){
        gArrowSpriteClips[0][0] = {26, 54, 30, 18}; gArrowSpriteClips[3][0] = {520, 54, 30, 18};
        gArrowSpriteClips[0][1] = {90, 54, 30, 18}; gArrowSpriteClips[3][1] = {456, 54, 30, 18};
        gArrowSpriteClips[0][2] = {152, 56, 32, 14}; gArrowSpriteClips[3][2] = {392, 56, 32, 14};
        gArrowSpriteClips[0][3] = {211, 56, 37, 15}; gArrowSpriteClips[3][3] = {328, 56, 37, 15};
        gArrowSpriteClips[0][4] = {278, 56, 34, 15}; gArrowSpriteClips[3][4] = {264, 56, 34, 15};
        gArrowSpriteClips[0][5] = {412, 46, 27, 42}; gArrowSpriteClips[3][5] = {200, 46, 27, 42};
        gArrowSpriteClips[0][6] = {462, 42, 31, 46}; gArrowSpriteClips[3][6] = {83, 42, 31, 46};
        gArrowSpriteClips[0][7] = {510, 47, 43, 39}; gArrowSpriteClips[3][7] = {23, 47, 43, 49};
        gArrowSpriteClipsSize[0] = 8; gArrowSpriteClipsSize[3] = 8;

        gArrowSpriteClips[1][0] = {96, 61, 16, 6}; gArrowSpriteClips[4][0] = {272, 61, 16, 6};
        gArrowSpriteClips[1][1] = {154, 61, 23, 5}; gArrowSpriteClips[4][1] = {207, 61, 23, 5};
        gArrowSpriteClips[1][2] = {214, 61, 27, 5}; gArrowSpriteClips[4][2] = {143, 61, 27, 5};
        gArrowSpriteClips[1][3] = {278, 61, 27, 5}; gArrowSpriteClips[4][3] = {79, 61, 27, 5};
        gArrowSpriteClips[1][4] = {341, 61, 28, 5}; gArrowSpriteClips[4][4] = {15, 61, 28, 5};
        gArrowSpriteClipsSize[1] = 5; gArrowSpriteClipsSize[4] = 5;
    }
    for(int i = 0; i < TOTAL_ENEMIES; i++){
        for(int j = 0; j < 12; j++){
            string tmp;
            if(j < 6){
                if(i == 0){
                    tmp = fileGhost0[j];
                }
                else{
                    tmp = fileGhost1[j];
                }
            }
            else{
                if(i == 0) tmp = fileGhost0[j - 6] + "_f";
                else tmp = fileGhost1[j - 6] + "_f";
            }
            string fname = "image/characters/" + fileGhost[i] + "/" + tmp + ".png";
            //cout << i << ' ' << j << ": " << fname << '\n';
            if(!gTEnemy[i][j].loadFromFile(fname, gRenderer)){
                cout << "Failed to load " << fileGhost[i] << "'s " << tmp << " state\n"; success = false;
            }
        }
    }
    if(success){
        gEnemySpriteClips[0][0][0].w = 48;
        gEnemySpriteClips[0][0][1].w = 30;
        gEnemySpriteClips[0][0][2].w = 50;
        gEnemySpriteClips[0][0][3].w = 42;
        for(int i = 0; i < 4; i++){
            gEnemySpriteClips[0][0][i].x = i * 127 + 55;
            gEnemySpriteClips[0][0][i].y = 57;
            gEnemySpriteClips[0][0][i].h = 71;
        }
        gEnemySpriteClipsSize[0][0] = 4;

        for(int i = 0; i < 6; i++){
            gEnemySpriteClips[0][1][i].x = i * 128 + 42;
            gEnemySpriteClips[0][1][i].w = 47;
            gEnemySpriteClips[0][1][i].h = 68;
            gEnemySpriteClips[0][1][i].y = 57;
        }
        gEnemySpriteClipsSize[0][1] = 6;

        for(int i = 0; i < 3; i++){
            gEnemySpriteClips[0][2][i].x = i * 125 + 46;
            gEnemySpriteClips[0][2][i].y = 57;
            gEnemySpriteClips[0][2][i].w = 34;
            gEnemySpriteClips[0][2][i].h = 68;
        }
        gEnemySpriteClipsSize[0][2] = 3;

        for(int i = 0; i < 7; i++){
            gEnemySpriteClips[0][3][i].x = i * 128 + 35; gEnemySpriteClips[0][4][i].x = i * 128 + 53;
            gEnemySpriteClips[0][3][i].y = 59; gEnemySpriteClips[0][4][i].y = 59;
            gEnemySpriteClips[0][3][i].h = 69; gEnemySpriteClips[0][4][i].h = 71;
            gEnemySpriteClips[0][3][i].w = 40; gEnemySpriteClips[0][4][i].w = 28;
            if(i == 6) gEnemySpriteClips[0][3][i].w += 4;

            gEnemySpriteClips[0][5][i].x = i * 127 + 54;
            gEnemySpriteClips[0][5][i].y = 57;
            gEnemySpriteClips[0][5][i].w = 24;
            gEnemySpriteClips[0][5][i].h = 71;
        }
        for(int i = 3; i <= 5; i++){
            gEnemySpriteClipsSize[0][i] = 7;
        }

        gEnemySpriteClips[1][0][0].w = 56;
        gEnemySpriteClips[1][0][1].w = 64;
        gEnemySpriteClips[1][0][2].w = 65;
        gEnemySpriteClips[1][0][3].w = 72;
        gEnemySpriteClips[1][0][4].w = 61;
        gEnemySpriteClips[1][0][5].w = 61;
        gEnemySpriteClips[1][0][6].w = 56;
        for(int i = 0; i < 7; i++){
            gEnemySpriteClips[1][0][i].x = i * 125 + 35;
            gEnemySpriteClips[1][0][i].y = 56;
            gEnemySpriteClips[1][0][i].h = 71;
        }
        gEnemySpriteClipsSize[1][0] = 7;

        for(int i = 0; i < 7; i++){
            gEnemySpriteClips[1][1][i].x = i * 128 + 35;
            gEnemySpriteClips[1][1][i].y = 58;
            gEnemySpriteClips[1][1][i].h = 70;
            gEnemySpriteClips[1][1][i].w = 60;
        }
        gEnemySpriteClipsSize[1][1] = 7;

        gEnemySpriteClips[1][2][0].w = 53; gEnemySpriteClips[1][2][1].w = 60; gEnemySpriteClips[1][2][2].w = 58;
        for(int i = 0; i < 3; i++){
            gEnemySpriteClips[1][2][i].y = 57;
            gEnemySpriteClips[1][2][i].h = 69;
            gEnemySpriteClips[1][2][i].x = i * 124 + 40;
        }
        gEnemySpriteClipsSize[1][2] = 3;

        for(int i = 0; i < 5; i++){
            gEnemySpriteClips[1][3][i].y = 49;
            gEnemySpriteClips[1][3][i].h = 71;
            gEnemySpriteClips[1][3][i].x = i * 127 + 33;
            gEnemySpriteClips[1][3][i].w = 58;
        }
        gEnemySpriteClipsSize[1][3] = 5;

        for(int i = 0; i < 4; i++){
            gEnemySpriteClips[1][4][i].y = 54;
            gEnemySpriteClips[1][4][i].h = 70;
            gEnemySpriteClips[1][4][i].w = 58;
            gEnemySpriteClips[1][4][i].x = i * 125 + 33;
        }
        gEnemySpriteClipsSize[1][4] = 4;

        for(int i = 0; i < 5; i++){
            gEnemySpriteClips[1][5][i].y = 53;
            gEnemySpriteClips[1][5][i].h = 70;
            gEnemySpriteClips[1][5][i].x = i * 127 + 33;
            gEnemySpriteClips[1][5][i].w = 38;
        }
        gEnemySpriteClipsSize[1][5] = 5;

        for(int i = 0; i < TOTAL_ENEMIES; i++){
            for(int j = 6; j < 12; j++){
                for(int k = 0; k < gEnemySpriteClipsSize[i][j - 6]; k++){
                    gEnemySpriteClips[i][j][k].x = gTEnemy[i][j - 6].getWidth() - gEnemySpriteClips[i][j - 6][k].x - gEnemySpriteClips[i][j - 6][k].w;
                    gEnemySpriteClips[i][j][k].w = gEnemySpriteClips[i][j - 6][k].w;
                    gEnemySpriteClips[i][j][k].h = gEnemySpriteClips[i][j - 6][k].h;
                    gEnemySpriteClips[i][j][k].y = gEnemySpriteClips[i][j - 6][k].y;
                }
                gEnemySpriteClipsSize[i][j] = gEnemySpriteClipsSize[i][j - 6];
            }
        }
    }
    string tmp = fileArrow[0];
    string fname = "image/characters/Yurei/" + tmp + ".png"; //cout << fname << '\n';
    if(!gTArrow[2].loadFromFile(fname, gRenderer)){
            cout << "Failed to load ghost's fire!\n"; success = false;
    }
    tmp += "_f";
    fname = "image/characters/Yurei/" + tmp + ".png";// cout << fname << '\n';
    if(!gTArrow[5].loadFromFile(fname, gRenderer)){
            cout << "Failed to load ghost's fire!\n"; success = false;
    }
    if(success){
        for(int i = 0; i <= 2; i++){
            gArrowSpriteClips[2][i].y = 8; gArrowSpriteClips[5][i].y = 8;
            gArrowSpriteClips[2][i].h = 8; gArrowSpriteClips[5][i].h = 8;
            gArrowSpriteClips[2][i].x = i * 24 + 5; gArrowSpriteClips[5][i].x = gTArrow[2].getWidth() - gArrowSpriteClips[2][i].x - gArrowSpriteClips[2][i].w;
            gArrowSpriteClips[2][i].w = 14; gArrowSpriteClips[5][i].w = 14;
        }
        gArrowSpriteClipsSize[2] = 3; gArrowSpriteClipsSize[5] = 3;
    }
    if(!gTLetter.loadFromFile("image/png/Object/letter.jpg", gRenderer)){
        success = false;
    }
    else{
        gLetter = {143, 32, 34, 23};
    }
    gTitleFont = TTF_OpenFont("fonts/Helmet.ttf", 60);
    gFont = TTF_OpenFont("fonts/Mgen+ 1pp regular.ttf", 24);
    if(gFont == NULL){
        cout << "Unable to load Mgen regular font! " << TTF_GetError() << '\n'; success = false;
    }
    gPauseFont1 = TTF_OpenFont("fonts/Mgen+ 1pp heavy.ttf", 50);
    gPauseFont2 = TTF_OpenFont("fonts/Mgen+ 1pp regular.ttf", 30);
    gHalloFont = TTF_OpenFont("fonts/Halloween.ttf", 60);
    for(int i = 1; i <= 18; i++){
        int tmp = i;
        string fpath = "image/png/Tiles/"; string id = to_string(tmp);
        fpath += id; fpath += ".png"; //cout << fpath << '\n';
        if(!gTypeTiles[i].loadFromFile(fpath, gRenderer)){
            cout << "Failed to load tiles texture!"; success = false;
        }
    }
    if(!gTMenuButtonsSpriteSheet[0].loadFromFile("image/paper/paper-button-off.png", gRenderer)){
        cout << "Failed to load on button!\n"; success = false;
    }
    if(!gTMenuButtonsSpriteSheet[1].loadFromFile("image/paper/paper-button-on.png", gRenderer)){
        cout << "Failed to load off button!\n"; success = false;
    }
    if(!gTEndLevelDialog.loadFromFile("image/paper/paper-dialog.png", gRenderer)){
        cout << "Failed to load dialog!\n"; success = false;
    }
    string ins_fname = "instruct.txt";
    fstream file(ins_fname, ios::in);
    vector<vector<string>> ins_content;
    vector<string> ins_row;
    string line, word;
    if(file.is_open()){
        while(getline(file, line)){
            ins_row.clear();
            stringstream str(line);
            while(getline(str, word)){
                ins_row.pb(word);
            }
            ins_content.pb(ins_row);
        }
    }
    else{
        cout << "Could not open file!"; success = false;
    }
    file.close();
    SDL_Color insColor = {119, 24, 24};
    for(int i = 0; i < (int)ins_content.size(); i++){
        string tmp = "";
        for(int j = 0; j < (int)ins_content[i].size(); j++){
            if(j > 0) tmp += " ";
            tmp += ins_content[i][j];
        }
        //cout << i << ": " << tmp << '\n';
        if(tmp != ""){
            if(!gTInstr[i].loadFromRenderedText(tmp, insColor, gRenderer, gFont)){
                cout << "Failed to display instruction!\n"; success = false; break;
            }
        }
    }

    menuMusic = Mix_LoadMUS("sounds/playingwithdolls.wav");
    playMusic_fight = Mix_LoadMUS("sounds/murdered.wav");
    playMusic_normal = Mix_LoadMUS("sounds/tchaikovsky.wav");
    gameOverMusic = Mix_LoadMUS("sounds/strange_ritual.wav");
    wonMusic = Mix_LoadMUS("sounds/arukas.wav");

    stab = Mix_LoadWAV("sounds/stab.wav");
    page = Mix_LoadWAV("sounds/page.wav");
    level_up = Mix_LoadWAV("sounds/level_up.wav");
    fall = Mix_LoadWAV("sounds/fall.wav");
    die = Mix_LoadWAV("sounds/die.wav");
    player_hurt = Mix_LoadWAV("sounds/player_hurt.wav");
    ghost_die[0] = Mix_LoadWAV("sounds/growl.wav");
    ghost_die[1] = Mix_LoadWAV("sounds/Scream.wav");
    ghost_att = Mix_LoadWAV("sounds/att.wav");
    sound_e = Mix_LoadWAV("sounds/sound_E.wav");//cout << "Ok!\n";
    sound_q = Mix_LoadWAV("sounds/sound_q.wav");
    sound_s = Mix_LoadWAV("sounds/sound_s.wav");
    button = Mix_LoadWAV("sounds/button.wav");

    return success;
}

bool setTiles(Tile* tiles[], string mapName){
    bool success = true;
    string fmap = "image/png/Map/map_" + mapName + ".csv";
    fstream file(fmap);
    vector<string> map_row;
    vector<vector<string>> map_content;
    string line, stringId;
    if(file.is_open()){
        while(getline(file, line)){
            map_row.clear();
            stringstream str(line);
            while(getline(str, stringId, ',')){
                map_row.pb(stringId);
            }
            map_content.pb(map_row);
        }
    }
    file.close();
    ground.clear(); coor.clear(); is_mv.clear();
    int tileId = 0;int li = -1; int lj = -1;
    //ground.pb(make_pair(make_pair(0, 9 * 32), 18 * 32));
    int lev;
    for(int i =0; i < 2; i++){
        if(levels[i] == mapName){
            lev = i; break;
        }
    }
    if(lv == "1-1"){
        ground.pb(mp(mp(0, 9 * TILE_WIDTH), 18 * TILE_HEIGHT));
    }
    for(int i = 0; i < (int)map_content.size(); i++){
        for(int j = 0; j < (int)map_content[i].size(); j++){
            stringId = map_content[i][j];
            int intId = stoi(stringId); intId -= 14;
            if((intId < 1 || intId > 18) && intId != -15){
                cout << "Invalid tilemap\n"; success = false; break;
            }
            else{
                if(intId == -15) intId = -1;
                if(intId == 11){
                    li = i; lj = j;
                }
                if(intId == 7){
                    ground.pb(make_pair(make_pair(lj * TILE_WIDTH, j * TILE_WIDTH), i * TILE_HEIGHT));
                    coor.pb(make_pair(make_pair(lj, li), make_pair(j, i)));
                    is_mv[mp(mp(lj * TILE_WIDTH, j * TILE_WIDTH), i * TILE_HEIGHT)] = 0;
                    li = -1; lj = -1;
                }
                if(intId == 13){
                    li = i; lj = j;
                }
                if(intId == 15){
                    ground.pb(make_pair(make_pair(lj * TILE_WIDTH, j * TILE_WIDTH), i * TILE_HEIGHT));
                    is_mv[mp(mp(lj * TILE_WIDTH, j * TILE_WIDTH), i * TILE_HEIGHT)] = 1;
                    if(lv == "1-1"){
                        if((int)upDown.size() < 4){
                            movingGround* tmp = NULL;
                            tmp = new movingGround(lev, lj * TILE_WIDTH, (j + 1) * TILE_WIDTH, i * TILE_HEIGHT, lj * TILE_WIDTH, (j + 1) * TILE_WIDTH, 128, 576, "ud");
                            tmp->setDeltaVel((int)upDown.size() * 2);
                            upDown.pb(tmp);
                            udId.pb((int)ground.size() - 1);
                        }
                    }
                    else{
                        //cout << ground.size() - 1 << ' ' <<  j << '\n';
                        if(j != 130 && j != 181){
                            movingGround* tmp = NULL;
                            tmp = new movingGround(lev, lj * TILE_WIDTH, (j + 1) * TILE_WIDTH, i * TILE_HEIGHT, lj * TILE_WIDTH, (j + 1) * TILE_WIDTH, 160, 15 * 32, "ud");
                            tmp->setDeltaVel(((int)upDown.size() * 2) % 3);
                            upDown.pb(tmp);
                            udId.pb((int)ground.size() - 1);
                        }
                        else if(j == 130){
                            movingGround* tmp = NULL;
                            tmp = new movingGround(lev, lj * TILE_WIDTH, (j + 1) * TILE_WIDTH, i * TILE_HEIGHT, 120 * TILE_WIDTH, 131 * TILE_WIDTH, i * TILE_HEIGHT, i * TILE_HEIGHT, "lr");
                            tmp->setDeltaVel(1);
                            leftRight.pb(tmp);
                            lrId.pb((int)ground.size() - 1);
                        }
                        else{
                            movingGround* tmp = NULL;
                            tmp = new movingGround(lev, lj * TILE_WIDTH, (j + 1) * TILE_WIDTH, i * TILE_HEIGHT, 177 * TILE_WIDTH, 185 * TILE_WIDTH, i * TILE_HEIGHT, i * TILE_HEIGHT, "lr");
                            tmp->setDeltaVel(0);
                            leftRight.pb(tmp);
                            lrId.pb((int)ground.size() - 1);
                        }
                        li = -1; lj = -1;
                    }
                }
                if(intId == 3){
                    ground.pb(mp(mp(0, j * TILE_WIDTH), i * TILE_HEIGHT));
                    is_mv[mp(mp(0, j * TILE_WIDTH), i * TILE_HEIGHT)] = 0;
                }
                if(intId == 1){
                    ground.pb(mp(mp(j * TILE_WIDTH, LEVEL_WIDTH[lev] - TILE_WIDTH), i * TILE_HEIGHT));
                    is_mv[mp(mp(j * TILE_WIDTH, LEVEL_WIDTH[lev] - TILE_WIDTH), i * TILE_HEIGHT)] = 0;
                }
                tiles[tileId] = new Tile(j * TILE_WIDTH, i * TILE_HEIGHT, intId); tileId++;
            }
        }
        if(success == false){
            break;
        }
    }
    //for(auto it : is_mv){
    //    cout << it.fi.fi.fi << ' ' << it.fi.fi.se << ' ' << it.fi.se << ": " << it.se << '\n';
    //}
    tot_tiles = tileId;
    return success;
}

void setGhosts(string lv){
    //cout << coor.size() << ' ' << ground.size() << '\n';
    srand(time(NULL));
    int noE = 2;
    if(lv != "1-1"){
        noE = 3;
    }
    for(int i = 0; i < TOTAL_ENEMIES; i++){
        for(int j = 0; j < noE; j++){
            int id = rand() % (int)coor.size();
            int lx = coor[id].fi.fi; int y = coor[id].fi.se; int rx = coor[id].se.fi + 1;
            Enemy* e = new Enemy(lx * TILE_WIDTH, y * TILE_HEIGHT, rx * TILE_WIDTH, i, gEnemySpriteClips[i][5][0]);
            ghost[i].pb(e);
        }
    }
    coor.clear();
    if(lv == "1-2"){
        for(int i = 0; i < (int)ground.size(); i++){
            if(ground[i].fi.se == LEVEL_WIDTH[1] - TILE_WIDTH){
                //cout << "Fin ground:    " << i << '\n';
                for(int ii = 0; ii < TOTAL_ENEMIES; ii++){
                    for(int c = 0; c < 4; c++){
                        Enemy* e = new Enemy(ground[i].fi.fi, ground[i].se, ground[i].fi.se, ii, gEnemySpriteClips[ii][5][0]);
                        int f = c * 48; //cout << f << ' ';
                        e->setDeltaX(f);
                        ghost[ii].pb(e);
                    }
                }
            }
        }
    }
}

void clear(Tile* tiles[]){
    for(int i = 0; i < MAX_TOTAL_TILES; i++){
        if(tiles[i] != NULL){
            delete tiles[i];
            tiles[i] = NULL;
        }
	}
	if(klee != NULL){
        delete klee; klee = NULL;
	}
	for(int i = 0; i < TOTAL_ENEMIES; i++){
        int j = 0;
        while(j < (int)ghost[i].size()){
            Enemy* e = ghost[i].at(j);
            if(e != NULL){
                ghost[i].erase(ghost[i].begin() + j);
                delete e; e = NULL;
                j--;
            }
            j++;
        }
	}
	int i = 0;
	//cout << "Mv: " << (int)upDown.size() << '\n';
	while(i < (int)upDown.size()){
        movingGround* mv = upDown.at(i);
        if(mv != NULL){
            upDown.erase(upDown.begin() + i);
            delete mv; mv = NULL;
            i--;
        }
        i++;
	}
	i = 0;
	while(i < (int)leftRight.size()){
        movingGround* mv = leftRight.at(i);
        if(mv != NULL){
            leftRight.erase(leftRight.begin() + i);
            delete mv; mv = NULL;
            i--;
        }
        i++;
	}
	//cout << (int)upDown.size() << ' ' << (int)leftRight.size();
	ground.clear(); coor.clear(); is_mv.clear();
	udId.clear(); lrId.clear();
	tot_tiles = 0;
}

void setupData(){
    string fname = "data.txt";
    fstream file(fname);
    vector<string> row;
    vector<vector<string>> content;
    string line, stringId;
    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            stringstream str(line);
            while(getline(str, stringId, ' ')){
                row.pb(stringId);
            }
            content.pb(row);
        }
        file.close();
    }
    //cleared level
    int cleared = stoi(content[1][0]); curLevelId = stoi(content[1][1]); st_level = stoi(content[1][2]);
    for(int i = (int)clearedLevel.size(); i < cleared; i++){
        clearedLevel.pb(levels[i]);
    }
    //ghosts
    int sz[2];
    sz[0] = stoi(content[2][0]);  sz[1] = stoi(content[2][1]);
    int curline = 3;
    for(int t = 0; t < TOTAL_ENEMIES; t++){
        for(int i = 0; i < sz[t]; i++){
            Enemy* e = new Enemy(stoi(content[curline][0]), stoi(content[curline][2]), stoi(content[curline][1]), t, gEnemySpriteClips[t][5][0]);
            int f = stoi(content[curline][4]);
            e->setDeltaX(f);
            ghost[t].pb(e);
            int cur_health = stoi(content[curline][3]);
            e->setHealth(cur_health);
            curline++;
        }
    }
}

void updateData(){
    for(int ty = 0; ty < TOTAL_ENEMIES; ty++){
        int j = 0;
        while(j < (int)ghost[ty].size()){
            Enemy* e = ghost[ty].at(j);
            if(e->getHealth() <= 0){
                ghost[ty].erase(ghost[ty].begin() + j);
                delete e; e = NULL;
                j--;
            }
            j++;
        }
    }
    string fname = "data.txt";
    fstream file;
    file.open(fname, ios::app);
    if(file){
        string cl = to_string((int)clearedLevel.size()) + " " + to_string(curLevelId) + " " + to_string(st_level) + "\n"; // << cl;
        file << cl;
        string numG = to_string((int)ghost[0].size()) + " " + to_string((int)ghost[1].size()) + "\n";// cout << numG;
        file << numG;
        for(int ty = 0; ty < TOTAL_ENEMIES; ty++){
            for(int i = 0; i < (int)ghost[ty].size(); i++){
                string line = to_string(ghost[ty][i]->getEnemyLeftX())
                                + " " + to_string(ghost[ty][i]->getEnemyRightX()) + " " + to_string(ghost[ty][i]->getEnemyGround())
                                + " " + to_string(ghost[ty][i]->getHealth()) + " " + to_string(ghost[ty][i]->getDeltaX()) + "\n";
                //cout << line;
                file << line;
            }
        }
        file.close();
    }
}

void showData(){
    string fname = "data.txt";
    fstream file(fname);
    vector<string> row;
    vector<vector<string>> content;
    string line, stringId;
    if(file.is_open()){
        while(getline(file, line)){
            row.clear();
            stringstream str(line);
            while(getline(str, stringId, ' ')){
                row.pb(stringId);
            }
            content.pb(row);
        }
        file.close();
    }
    for(int i = 0; i < (int)content.size(); i++){
        for(int j = 0; j < (int)content[i].size(); j++){
            cout << content[i][j] << ' ';
        }
        cout << '\n';
    }
}

void close(Tile* tiles[]){
	//Free loaded image
	gTBackground.free();
	for(int i = 0; i < 18; i++){
        gTKlee[i].free();
	}
	for(int i = 0; i < INSTR_LENGTH; i++){
        gTInstr[i].free();
	}
    gTKleeHb.free();
    gTKleeHbBackground.free();
    gTKleeExp.free();
    gTKleeExpBg.free();
    for(int i = 0; i < TOTAL_ENEMIES; i++){
        for(int j = 0; j < 12; j++){
            gTEnemy[i][j].free();
        }
    }
    for(int i = 0; i < 6; i++){
        gTArrow[i].free();
    }
    for(int i = 0; i < 2; i++){
        gTMenuButtonsSpriteSheet[i].free();
    }
    LTexture gTypeTiles[TOTAL_TILE_TYPES];

    gTText.free();
    gTEndLevelSign.free();
    gTEndLevelDialog.free();
    gTWinText.free();
    gTLoseText.free();
    gTFailText.free();
    gTTime.free();
    gTExpText.free();
    gTEnemyHb.free();
    gTEnemyHbBg.free();
    gTLevelDialog.free();
    gTUlt.free();
    gTUltCD.free();
    gTUltTime.free();
    gTLetter.free();
    gTPreIns.free();
    gTSmallDialog.free();

    TTF_CloseFont(gFont); gFont = NULL;
    TTF_CloseFont(gTitleFont); gTitleFont = NULL;
    TTF_CloseFont(gPauseFont1); gPauseFont1 = NULL;
    TTF_CloseFont(gPauseFont2); gPauseFont2 = NULL;
    TTF_CloseFont(gHalloFont); gHalloFont = NULL;

    Mix_FreeMusic(menuMusic);
    menuMusic = NULL;
    Mix_FreeMusic(playMusic_fight);
    playMusic_fight = NULL;
    Mix_FreeMusic(playMusic_normal);
    playMusic_normal = NULL;
    Mix_FreeMusic(gameOverMusic);
    gameOverMusic = NULL;
    Mix_FreeMusic(wonMusic);
    wonMusic = NULL;

    Mix_FreeChunk(stab);
    stab = NULL;
    Mix_FreeChunk(page);
    page = NULL;
    Mix_FreeChunk(level_up);
    level_up = NULL;
    Mix_FreeChunk(fall);
    fall = NULL;
    Mix_FreeChunk(die);
    die = NULL;
    Mix_FreeChunk(player_hurt);
    player_hurt = NULL;
    Mix_FreeChunk(ghost_die[0]);
    ghost_die[0] = NULL;
    Mix_FreeChunk(ghost_die[1]);
    ghost_die[1] = NULL;
    Mix_FreeChunk(ghost_att);
    ghost_att = NULL;
    Mix_FreeChunk(sound_e);
    sound_e = NULL;
    Mix_FreeChunk(sound_q);
    sound_q = NULL;
    Mix_FreeChunk(sound_s);
    sound_s = NULL;
    Mix_FreeChunk(button);
    button = NULL;

    if(letter != NULL){
        delete letter;
    }
    clear(tiles);
    int i = 0;
	while(i < (int)menuStart.size()){
        Buttons* bt = menuStart.at(i);
        if(bt != NULL){
            menuStart.erase(menuStart.begin() + i);
            delete bt; bt = NULL;
            i--;
        }
        i++;
	}
	i = 0;
    while(i < (int)gameOverButtons.size()){
        Buttons* bt = gameOverButtons.at(i);
        if(bt != NULL){
            gameOverButtons.erase(gameOverButtons.begin() + i);
            delete bt; bt = NULL;
            i--;
        }
        i++;
	}
	i = 0;
    while(i < (int)wonButtons.size()){
        Buttons* bt = wonButtons.at(i);
        if(bt != NULL){
            wonButtons.erase(wonButtons.begin() + i);
            delete bt; bt = NULL;
            i--;
        }
        i++;
	}
	i = 0;
    while(i < (int)failedButtons.size()){
        Buttons* bt = failedButtons.at(i);
        if(bt != NULL){
            failedButtons.erase(failedButtons.begin() + i);
            delete bt; bt = NULL;
            i--;
        }
        i++;
	}
    clearedLevel.clear();
}
