FROM gitpod/workspace-full

RUN sudo apt-get update \
    && sudo apt-get dist-upgrade -y \
    && sudo apt-get install -y \
        strace netcat-openbsd lazygit \
    && sudo rm -rf /var/lib/apt/lists/*
