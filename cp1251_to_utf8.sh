cat $1 | iconv -f CP1251 -t koi8-r | catdoc -d us-ascii -s koi8-r >> /tmp/$$.cv && mv /tmp/$$.cv $1
#cat $1 | iconv -f CP1251 -t koi8-r >> /tmp/$$.cv && mv /tmp/$$.cv $1
#cat $1 | iconv -f CP1251 -t UTF-8 >> /tmp/$$.cv && mv /tmp/$$.cv $1
