# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: vzurera- <vzurera-@student.42malaga.com    +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/08/11 19:13:18 by vzurera-          #+#    #+#              #
#    Updated: 2025/08/26 10:06:54 by vzurera-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

all: MattDaemon Ben_AFK Casey_AFK

MattDaemon: _daemon
server: _daemon
_daemon:
	@mkdir -p bin
	@$(MAKE) -s -C MattDaemon all
	@cp MattDaemon/MattDaemon bin/

Ben_AFK: _client
_client:
	@mkdir -p bin
	@$(MAKE) -s -C Ben_AFK all
	@cp Ben_AFK/Ben_AFK bin/

Casey_AFK: _client_gui
_client_gui:
	@mkdir -p bin
	@$(MAKE) -s -C Casey_AFK all
	@cp Casey_AFK/Casey_AFK bin/

re:
	@$(MAKE) -s -C MattDaemon re
	@$(MAKE) -s -C Ben_AFK re
	@$(MAKE) -s -C Casey_AFK re

clean:
	@$(MAKE) -s -C MattDaemon clean
	@$(MAKE) -s -C Ben_AFK clean
	@$(MAKE) -s -C Casey_AFK clean

fclean:
	@$(MAKE) -s -C MattDaemon fclean
	@$(MAKE) -s -C Ben_AFK fclean
	@$(MAKE) -s -C Casey_AFK fclean
	@rm -rf bin

wipe:
	@$(MAKE) -s -C MattDaemon wipe
	@$(MAKE) -s -C Ben_AFK wipe
	@$(MAKE) -s -C Casey_AFK wipe
	@rm -rf bin

.PHONY: all MattDaemon Ben_AFK Casey_AFK _daemon _client _client_gui fclean wipe re
