# ===========================================================
# ZIMEX - Brotato-style top-down shooter
# Binaire : zimex
# Lib     : raylib
# Install : sudo apt install libraylib-dev   (Linux)
#           brew install raylib              (macOS)
# ===========================================================

NAME     = zimex
CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -I./include
LDFLAGS  = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

SRC_DIR = src
OBJ_DIR = obj

SRC = $(SRC_DIR)/GlobalComponent.cpp  \
      $(SRC_DIR)/RenderSystem.cpp     \
      $(SRC_DIR)/InputSystem.cpp      \
      $(SRC_DIR)/CollisionSystem.cpp  \
      $(SRC_DIR)/PhysicSystem.cpp     \
      $(SRC_DIR)/HealthSystem.cpp     \
      $(SRC_DIR)/AnimationSystem.cpp  \
      $(SRC_DIR)/WeaponSystem.cpp     \
      $(SRC_DIR)/BulletSystem.cpp     \
      $(SRC_DIR)/EnemySystem.cpp      \
      $(SRC_DIR)/WaveSystem.cpp       \
      $(SRC_DIR)/SoundSystem.cpp      \
      $(SRC_DIR)/Game.cpp             \
      main.cpp

OBJ = $(SRC:%.cpp=$(OBJ_DIR)/%.o)

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $@ $^ $(LDFLAGS)
	@echo "\033[0;32m✓ zimex built successfully\033[0m"

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	@echo "\033[0;33m✓ Objects cleaned\033[0m"

fclean: clean
	rm -f $(NAME)
	@echo "\033[0;31m✓ zimex removed\033[0m"

re: fclean all
run: all
	./$(NAME)

.PHONY: all clean fclean re run
