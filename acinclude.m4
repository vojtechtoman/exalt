AC_DEFUN(AC_CHECK_SIZEOF_XMLCHAR,
[
  AC_MSG_CHECKING([size of XML_Char])
  AC_TRY_RUN(
  [
    #include <expat.h>
    #include <fstream>

    int main(int argc, char **argv)
    {
      ofstream f;
      f.open("XXexpatXX");
      f << sizeof(XML_Char);
      f.close();
      return 0;
    }
  ],
  [
    XML_Char_Size=`cat XXexpatXX`
    rm -f XXexpatXX
    AC_MSG_RESULT(${XML_Char_Size})
    AC_DEFINE_UNQUOTED([SIZEOF_XML_CHAR], ${XML_Char_Size})
  ],
  [
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Unable to run test code! See config.log for details.])
  ],
  [
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Unable to run test code! See config.log for details.])
  ],
)])


AC_DEFUN(AC_CHECK_EXPAT_VERSION,
[
  AC_MSG_CHECKING([version of Expat])
  AC_TRY_RUN(
  [
    #include <expat.h>
    #include <fstream>

    int main(int argc, char **argv)
    {
      ofstream f;

      f.open("XXexpatXX");
      XML_Expat_Version ver =  XML_ExpatVersionInfo();
      f << ver.major << " ";
      f << ver.minor << " ";
      f << ver.micro << " ";
      f.close();
      return 0;
    }
  ],
  [
    read Expat_Major Expat_Minor Expat_Micro < XXexpatXX
    rm -f XXexpatXX
    AC_MSG_RESULT(${Expat_Major}.${Expat_Minor}.${Expat_Micro})
    AC_DEFINE_UNQUOTED([EXPAT_VERSION], "${Expat_Major}.${Expat_Minor}.${Expat_Micro}")

    if test ${Expat_Major}${Expat_Minor}${Expat_Micro} -lt 1955; then
      AC_MSG_ERROR([You need Expat 1.95.5 or higher in order to continue!])
    fi
  ],
  [
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Unable to run test code! See config.log for details.])
  ],
  [
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([Unable to run test code! See config.log for details.])
  ],
)])

