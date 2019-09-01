#include "SoundManager.h"

 void SoundMgr::customFatalError(wxString message)
	{
	    wxTheApp->GetTopWindow()->Close();
		wxTheApp->ExitMainLoop();

		wxMessageBox(message);

		exit(-1);
	}
