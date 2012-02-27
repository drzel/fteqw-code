/*
this file is basically a copy of the SDL one
java code has a function or two which just periodically calls us to ask us to dump out audio for it
*/
#include "quakedef.h"
#include <jni.h>
#include <pthread.h>

//static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static soundcardinfo_t *sys_sc = NULL;



//transfer the 'dma' buffer into the buffer it requests, called from a dedicated sound thread created by the java code.
JNIEXPORT jint JNICALL Java_com_fteqw_FTEDroidEngine_paintaudio(JNIEnv *env, jclass this, jbyteArray stream, jint len)
{
	int offset = 0;
	soundcardinfo_t *sc = sys_sc;
	int framesz;
//	if (!pthread_mutex_lock(&mutex))
	{
		if (sc)
		{
			int buffersize = sc->sn.samples*sc->sn.samplebits/8;

			int curtime = GetSoundtime(sc);
			framesz = sc->sn.numchannels * sc->sn.samplebits/8;

			S_PaintChannels (sc, curtime + (len / framesz));

			if (len > buffersize)
			{
				len = buffersize;       //whoa nellie!
			}

			if (len + sc->snd_sent%buffersize > buffersize)
			{	//buffer will wrap, fill in the rest
				(*env)->SetByteArrayRegion(env, stream, offset, buffersize - (sc->snd_sent%buffersize), (char*)sc->sn.buffer + (sc->snd_sent%buffersize));
				offset += buffersize - (sc->snd_sent%buffersize);
				sc->snd_sent += buffersize - (sc->snd_sent%buffersize);
				len -= buffersize - (sc->snd_sent%buffersize);
				if (len < 0) /*this must be impossible, surely?*/
					len = 0;
			}
			//and finish from the start
			(*env)->SetByteArrayRegion(env, stream, offset, len, (char*)sc->sn.buffer + (sc->snd_sent%buffersize));
			offset += len;
			sc->snd_sent += len;
		}
		else
			offset = len;	/*so the playback thread ends up blocked properly*/
//		pthread_mutex_unlock(&mutex);
	}
	return offset;
}


static void Droid_Shutdown(soundcardinfo_t *sc)
{
//	pthread_mutex_lock(&mutex);

	sys_sc = NULL;
	free(sc->sn.buffer);
	
//	pthread_mutex_unlock(&mutex);
}

static unsigned int Droid_GetDMAPos(soundcardinfo_t *sc)
{
	sc->sn.samplepos = sc->snd_sent / (sc->sn.samplebits/8);
	return sc->sn.samplepos;
}

static void Droid_UnlockBuffer(soundcardinfo_t *sc, void *buffer)
{
//	pthread_mutex_unlock(&mutex);
}

static void *Droid_LockBuffer(soundcardinfo_t *sc)
{
//	pthread_mutex_lock(&mutex);
        return sc->sn.buffer;
}

static void Droid_SetUnderWater(soundcardinfo_t *sc, qboolean uw)
{
}

static void Droid_Submit(soundcardinfo_t *sc, int start, int end)
{
}

static int Droid_InitCard (soundcardinfo_t *sc, int cardnum)
{
	if (sys_sc)
		return 2;

//	if (!pthread_mutex_lock(&mutex))
	{
		sc->selfpainting = true;
		sc->sn.speed = 11025;
		sc->sn.samplebits = 16;
		sc->sn.numchannels = 1;
		/*internal buffer should have 1 sec audio*/
		sc->sn.samples = sc->sn.speed*sc->sn.numchannels;

		sc->Lock = Droid_LockBuffer;
		sc->Unlock = Droid_UnlockBuffer;
		sc->SetWaterDistortion = Droid_SetUnderWater;
		sc->Submit = Droid_Submit;
		sc->Shutdown = Droid_Shutdown;
		sc->GetDMAPos = Droid_GetDMAPos;

		sc->sn.buffer = malloc(sc->sn.samples*sc->sn.samplebits/8);
		
		sys_sc = sc;
		
//		pthread_mutex_unlock(&mutex);

		return 1;
	}
	return 0;
}
int (*pDroid_InitCard) (soundcardinfo_t *sc, int cardnum) = &Droid_InitCard;

