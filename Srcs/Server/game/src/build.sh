echo "### Metin2 Game Builder ###"
echo "### Cleaning ...."
echo "### Building ###"
gmake -j20 > ./LIB_BUILD_LOG.txt 2> ./ERROR_LOG.txt
echo "### Done Building ###"
