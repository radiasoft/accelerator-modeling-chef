   /* Qt macros on */
     

#ifdef  QT_MOC_CPP
#define slots                       slots
#define signals		            signals
#else
#define signals                     protected
#define slots                       
#endif
