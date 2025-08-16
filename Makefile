# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/11 19:13:18 by vzurera-          #+#    #+#              #
#    Updated: 2025/08/16 18:31:30 by vzurera-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

all: MattDaemon Ben_AFK

MattDaemon: daemon
daemon:
	@mkdir -p bin
	@$(MAKE) -s -C Daemon all && cp Daemon/MattDaemon bin/

Ben_AFK: client
client:
	@mkdir -p bin
	@$(MAKE) -s -C Ben_AFK all && cp Ben_AFK/Ben_AFK bin/

fclean: wipe
wipe:
	@$(MAKE) -s -C Daemon wipe
	@$(MAKE) -s -C Ben_AFK wipe
	@rm -rf bin

.PHONY: all MattDaemon daemon Ben_AFK client fclean wipe 
