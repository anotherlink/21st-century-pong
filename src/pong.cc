// original Copyright [2015] <Chafic Najjar>
//
// modified / adapted [2018] by photon@nailara.net

#include "src/pong.h"

#include <iostream>

#include "src/ball.h"
#include "src/paddle.h"
#include "src/utilities.h"

int Pong::SCREEN_WIDTH;
int Pong::SCREEN_HEIGHT;

Pong::Pong(int argc, char *argv[]) {

    int volume = MIX_MAX_VOLUME / 3;

    win_left   = atoi(argv[1]);
    win_top    = atoi(argv[2]);
    win_width  = atoi(argv[3]);
    win_height = atoi(argv[4]);

    Pong::SCREEN_WIDTH  = win_width;
    Pong::SCREEN_HEIGHT = win_height;

    SDL_Init(SDL_INIT_EVERYTHING);

    SDL_SetHint(SDL_HINT_GRAB_KEYBOARD, "0" );
    SDL_SetHint(SDL_HINT_MOUSE_NORMAL_SPEED_SCALE,  "0");
    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_SPEED_SCALE,"0");
    SDL_SetHint(SDL_HINT_MOUSE_RELATIVE_MODE_WARP,  "1");

    SDL_ShowCursor(0);

    SDL_CaptureMouse(SDL_TRUE);



    window = SDL_CreateWindow( "pong-game",
                                win_left,
                                win_top,
                                win_width,
                                win_height,
                                SDL_WINDOW_SHOWN
                                | SDL_WINDOW_BORDERLESS
                                | SDL_WINDOW_INPUT_GRABBED
                                | SDL_WINDOW_MOUSE_CAPTURE );

    SDL_SetWindowGrab(window, SDL_TRUE);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED
                                            | SDL_RENDERER_PRESENTVSYNC );

    SDL_RenderSetLogicalSize(renderer, win_width, win_height);

    // Instantiate game objects.
    ball = new Ball(win_width/2-ball->LENGTH/2,
            win_height/2-ball->LENGTH/2);
    left_paddle = new Paddle(40, win_height/2-Paddle::HEIGHT/2);
    right_paddle = new Paddle(win_width-(40+Paddle::WIDTH),
            win_height/2-Paddle::HEIGHT/2);

    // Sounds.

    // Initialize SDL_mixer.
    Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 1024);
    Mix_Volume( -1, volume );

    // Load paddle sound.
    paddle_sound = Mix_LoadWAV("resources/sounds/paddle_hit.wav");

    // Load wall sound.
    wall_sound = Mix_LoadWAV("resources/sounds/wall_hit.wav");

    // Load score sound.
    score_sound = Mix_LoadWAV("resources/sounds/score_update.wav");

    // Controllers.
    if (argc == 6) {
        if ( strcmp(argv[5], "joystick") == 0 ) {
            controller = joystick;
        } else {
            controller = mouse;
        }
    } else {
        controller = mouse;  // Default controller.
    }

    if (controller == joystick) {
        printf("[joysticks_found]\n%i\n\n", SDL_NumJoysticks() );
        printf("[joystick_names]\n");

        // Give control to the first joystick.
        gamepad = SDL_JoystickOpen(0);
        for (int i = 0; i < SDL_NumJoysticks(); i++) {
            std::cout << "\t" << SDL_JoystickName(gamepad) << std::endl;
        }

       // Initialize joystick controller.
        SDL_JoystickEventState(SDL_ENABLE);

        gamepad_direction = 0;
    }

    int press_ok_size = int( win_width / 33 );

    // Fonts.
    TTF_Init();  // Initialize font.
    font_color = {0, 255, 0, 255};
    font_name = "resources/fonts/NES-Chimera/NES-Chimera.ttf";
    font_image_launch = renderText("p r e s s  OK  t o  p l a y",
            font_name, font_color, press_ok_size, renderer);

    // Scores.
    left_score  = 0;
    right_score = 0;

    // Indicates when rendering new score is necessary.
    left_score_changed  = true;
    right_score_changed = true;

    // Game status.
    exit = false;
}

Pong::~Pong() {
    // Destroy textures.
    SDL_DestroyTexture(font_image_left_score);
    SDL_DestroyTexture(font_image_right_score);
    SDL_DestroyTexture(font_image_winner);
    SDL_DestroyTexture(font_image_restart);
    SDL_DestroyTexture(font_image_launch);

    // Free sound effects.
    Mix_FreeChunk(paddle_sound);
    Mix_FreeChunk(wall_sound);
    Mix_FreeChunk(score_sound);

    // Quit SDL_mixer.
    Mix_CloseAudio();

    // Close joystick.
    if (controller == joystick) {
        SDL_JoystickClose(gamepad);
    }

    // Destroy renderer and window.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    // Shuts down SDL.
    SDL_Quit();
}

void Pong::execute() {
    while (!exit) {
        input();
        update();
        render();
        SDL_Delay(9);
    }
}

void Pong::input() {
    // Handle events.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        bool fullscreen = 0;
        int flags = SDL_GetWindowFlags(window);
        if ( flags & SDL_WINDOW_FULLSCREEN ) {
            fullscreen = 1;
        } else {
            fullscreen = 0;
        }

        // Track mouse movement.
        if (event.type == SDL_MOUSEMOTION) {
            SDL_GetMouseState(&mouse_x, &mouse_y);
        }

        // Clicking 'x' or pressing F4.
        if (event.type == SDL_QUIT) {
            printf("[terminated]\n");
            exit = true;
        }

        // Joystick direction controller moved.
        if (event.type == SDL_JOYAXISMOTION) {
            // 32767.
            // Up or down.
            if (event.jaxis.axis == 1) {
                gamepad_direction = event.jaxis.value/5461;
            }
        }

        // KEYBOARD INPUT
        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                // Pressing ESC exits from the game.
                case SDLK_ESCAPE:
                    printf("[quit]\n");
                    exit = true;
                    break;
            }
        }

        // processing BUTTON press events
        if (event.type == SDL_MOUSEBUTTONDOWN||event.type == SDL_JOYBUTTONDOWN){
            if (fullscreen) {                              // EXIT FULLSCREEN //
                if (ball->status != ball->READY) {
                    SDL_SetWindowFullscreen(window, 0);
                    SDL_RestoreWindow(window);
                    SDL_SetWindowSize(window, win_width, win_height);
                    SDL_SetWindowPosition(window, win_left, win_top);
                    ball->pause_delay = 23;
                }
            } else {                              // ACTIVATE FULLSCREEN MODE //
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
                ball->pause_delay = 33;
            }
            // START GAME / PLAYING //
            if (ball->status == ball->READY) {
                ball->status = ball->LAUNCH;
                printf("[start]\n");
            }
        }
    }
}

// Update game values.
void Pong::update() {

    bool sound = 1;                                          // <- PLAY SOUNDS ?

    bool fullscreen = 0;
    int flags = SDL_GetWindowFlags(window);
    if ( flags & SDL_WINDOW_FULLSCREEN ) {
        fullscreen = 1;
    } else {
        fullscreen = 0;
    }

    // Paddle movement.
    if (controller == mouse) {
        // Right paddle follows the player's mouse on the y-axis.
        right_paddle->set_y(mouse_y);
    } else if (controller == joystick) {
        // Right paddle follows the player's gamepad.
        right_paddle->add_to_y(gamepad_direction);
    }

    // AI paddle movement.
    if ( !ball->pause_delay ) {
        left_paddle->AI(ball);
    }

    // Launch ball.
    if (ball->status == ball->READY) {
        return;
    } else if (ball->status == ball->LAUNCH) {
        ball->launch_ball(left_paddle);
        ball->predicted_y = left_paddle->predict(ball);
    }

    // Update ball speed.
    ball->update_speed();

    // Collision.
    if (ball->collides_with(left_paddle)) {
        ball->bounces_off(left_paddle);
        if (sound && fullscreen) {
            Mix_PlayChannel(-1, paddle_sound, 0);  // Play collision sound.
        }
    } else if (ball->collides_with(right_paddle)) {
        ball->bounces_off(right_paddle);
        // Predict ball position on the y-axis.
        ball->predicted_y = left_paddle->predict(ball);
        if (sound && fullscreen) {
            Mix_PlayChannel(-1, paddle_sound, 0);
        }
    }

    // Upper and bottom walls collision.
    if (ball->wall_collision()) {
        ball->dy *= -1;  // Reverse ball direction on y-axis.
        if (sound && fullscreen) {
            Mix_PlayChannel(-1, wall_sound, 0);  // Play collision sound.
        }
    }

    // Update ball coordinates.
    if( !ball->pause_delay ) {
        ball->x += ball->dx;
        ball->y += ball->dy;
    } else {
        ball->pause_delay--;
    }

    // Ball goes out.
    if (ball->x > win_width || ball->x < 0) {
        // Change score.
        int flags = SDL_GetWindowFlags(window);
        if (flags & SDL_WINDOW_FULLSCREEN) {
            if (ball->x > win_width) {
                left_score++;
                left_score_changed = true;
            } else {
                right_score++;
                right_score_changed = true;
            }
        printf("<%i:%i>\n", left_score, right_score);
        } else {
            left_score  = 0;
            right_score = 0;
            printf("<%i:%i>\n", left_score, right_score);
            left_score_changed  = true;
            right_score_changed = true;
            printf("[ready]\n");
        }
        if (sound && fullscreen) {
            Mix_PlayChannel(-1, score_sound, 0);
        }
        ball->reset();
    }
}

// Render objects on screen.
void Pong::render() {
    // Clear screen (background color).
    SDL_SetRenderDrawColor(renderer, 0, 0, 9, 255);
    SDL_RenderClear(renderer);

    // Paddle color.
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    // Render filled paddle.
    SDL_Rect paddle1 = { left_paddle->get_x(),
        left_paddle->get_y(),
        Paddle::WIDTH, Paddle::HEIGHT };
    SDL_RenderFillRect(renderer, &paddle1);

    // Render filled paddle.
    SDL_Rect paddle2 = { right_paddle->get_x(),
        right_paddle->get_y(),
        Paddle::WIDTH, Paddle::HEIGHT };
    SDL_RenderFillRect(renderer, &paddle2);

    // Render ball.
    SDL_Rect pong_ball = { ball->x, ball->y, ball->LENGTH, ball->LENGTH };
    SDL_RenderFillRect(renderer, &pong_ball);

    int score_font_size   = int( win_width / 25 );
    int press_ok_size     = int( win_width / 33 );
    int player_won_size   = int( win_width / 32 );
    int won_continue_size = int( win_width / 50 );

    // Render scores.
    if (left_score_changed) {
        font_image_left_score = renderText(std::to_string(left_score),
                font_name,
                font_color,
                score_font_size,
                renderer);
        left_score_changed = false;
        if (left_score >= 5) {
            font_image_winner = renderText("Player 1 won! [ /o\\ ]",
                    font_name, font_color, player_won_size, renderer);
            font_image_restart=renderText( "p r e s s  OK  to play again.. >:]",
                    font_name, font_color, won_continue_size, renderer);
            printf("[player_1_won]\n");
        }
    }
    renderTexture(font_image_left_score,
            renderer, win_width * 4 / 10, win_height / 12);

    if (right_score_changed) {
        font_image_right_score = renderText(std::to_string(right_score),
                font_name, font_color, score_font_size, renderer);
        right_score_changed = false;
        if (right_score >= 5) {
            font_image_winner = renderText("Player 2 won! [*\\o/-]",
                    font_name, font_color, player_won_size, renderer);
            font_image_restart = renderText("p r e s s  OK  to play again.. =)",
                    font_name, font_color, won_continue_size, renderer);
            printf("[player_2_won]\n");
        }
    }
    renderTexture(font_image_right_score,
            renderer,
            win_width * 6 / 10 - score_font_size/2, win_height/ 12);

    // Render text indicating the winner.
    if (left_score >= 5) {
       // Align with score.
        renderTexture(font_image_winner,
                renderer, win_width * 1 / 10 + 3, win_height / 3.5);
        renderTexture(font_image_restart,
                renderer, win_width * 1 / 10 + 3, win_height / 2.5);
        if (ball->status == ball->LAUNCHED) {
            left_score = 0;
            right_score = 0;
            left_score_changed = true;
            right_score_changed = true;
        }
    } else if (right_score >= 5) {
       // Align with score.
        renderTexture(font_image_winner,
                renderer,
                win_width - 18.5 * score_font_size, win_height / 4);
        renderTexture(font_image_restart,
                renderer,
                win_width - 18.5 * score_font_size, win_height / 3);
        if (ball->status == ball->LAUNCHED) {
            left_score = 0;
            right_score = 0;
            left_score_changed = true;
            right_score_changed = true;
        }
    } else if (ball->status == ball->READY) {
        // SDL_SetWindowFullscreen(window, 0);
        // Draw "p r e s s  OK  t o  p l a y".
        renderTexture(font_image_launch, renderer,
                win_width / 2 - ( 28 * press_ok_size/2 ),
                win_height * 0.8 );
    }
    // Swap buffers.
    SDL_RenderPresent(renderer);
}
