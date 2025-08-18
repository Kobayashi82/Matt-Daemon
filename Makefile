# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/11 19:13:18 by vzurera-          #+#    #+#              #
#    Updated: 2025/08/18 23:00:32 by vzurera-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

all: MattDaemon Ben_AFK

MattDaemon: daemon
server: daemon
daemon:
	@clear
	@mkdir -p bin
	@$(MAKE) -s -C MattDaemon all
	@cp MattDaemon/MattDaemon bin/

Ben_AFK: client
client:
	@clear
	@mkdir -p bin
	@$(MAKE) -s -C Ben_AFK all
	@cp Ben_AFK/Ben_AFK bin/

re:
	@clear
	@$(MAKE) -s -C MattDaemon re
	@$(MAKE) -s -C Ben_AFK re

clean:
	@clear
	@$(MAKE) -s -C MattDaemon clean
	@$(MAKE) -s -C Ben_AFK clean

fclean:
	@clear
	@$(MAKE) -s -C MattDaemon fclean
	@$(MAKE) -s -C Ben_AFK fclean
	@rm -rf bin

wipe:
	@$(MAKE) -s -C MattDaemon wipe
	@$(MAKE) -s -C Ben_AFK wipe
	@rm -rf bin

.PHONY: all MattDaemon daemon server Ben_AFK client fclean wipe re
