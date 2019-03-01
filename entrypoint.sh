if [ -f "$CFG_PATH" ]; then

    # Get information from cfg/cfg.ini
    dbhost=$(sed -nr "/^\[DatabaseUser\]/ { :l /^host[ ]*=/ { s/.*=[ ]*//; p; q;}; n; b l;}" "$CFG_PATH")
    dbname=$(sed -nr "/^\[DatabaseUser\]/ { :l /^dbname[ ]*=/ { s/.*=[ ]*//; p; q;}; n; b l;}" "$CFG_PATH")
    dbuser=$(sed -nr "/^\[DatabaseUser\]/ { :l /^login[ ]*=/ { s/.*=[ ]*//; p; q;}; n; b l;}" "$CFG_PATH")
    dbpass=$(sed -nr "/^\[DatabaseUser\]/ { :l /^password[ ]*=/ { s/.*=[ ]*//; p; q;}; n; b l;}" "$CFG_PATH")
    dbport=$(sed -nr "/^\[DatabaseUser\]/ { :l /^port[ ]*=/ { s/.*=[ ]*//; p; q;}; n; b l;}" "$CFG_PATH")
    friendCoreDomain=$(sed -nr "/^\[FriendCore\]/ { :l /^fchost[ ]*=/ { s/.*=[ ]*//; p; q;}; n; b l;}" "$CFG_PATH")
    friendNetwork=$(sed -nr "/^\[FriendNetwork\]/ { :l /^enabled[ ]*=/ { s/.*=[ ]*//; p; q;}; n; b l;}" "$CFG_PATH")
    friendChat=$(sed -nr "/^\[FriendChat\]/ { :l /^enabled[ ]*=/ { s/.*=[ ]*//; p; q;}; n; b l;}" "$CFG_PATH")
    cfgFound="yes"
fi

( echo "select * from FGlobalVariables;" | \
  mysql -u ${dbuser} -p${dbpass} -h ${dbhost} ${dbname} || \
  mysql -u ${dbuser} -p${dbpass} -h ${dbhost} ${dbname} --execute="SOURCE db/FriendCoreDatabase.sql" ) && \
  ./FriendCore
