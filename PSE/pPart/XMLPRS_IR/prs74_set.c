#ifdef ACCL_SENSOR
void PRSSetAccl( void )
{
   {
  	sens_ADXL210Sample *accl;
        accl = sens_SSimpSample_add_accelleration(&s);

        AcclXSensorPrepare();
        AcclXSensorGetGravity(&(accl->x));
        AcclYSensorPrepare();
        AcclYSensorGetGravity(&(accl->y));
#ifdef ACCL_Z_SENSOR
        //TODO change to optional
        AcclZSensorPrepare();
        AcclZSensorGetGravity( &(accl->z) );
#endif
   }
}
#else
void PRSSetAccl( void ) {
  	sens_ADXL210Sample *accl;
        accl = sens_SSimpSample_add_accelleration(&s);

        accl->x=-1000;
        accl->y=1000;
        accl->z=2000;
}
#endif

#ifdef AUDIO_SENSOR
void PRSSetAudio( void )
{
   sens_SP101Sample *audio;
   audio = sens_SSimpSample_add_audio(&s);

   AudioSensorPrepare();
   AudioSensorGetVolume( &(audio->volume) );
}
#else
void PRSSetAudio( void ) {
   sens_SP101Sample *audio;
   audio = sens_SSimpSample_add_audio(&s);

   audio->volume=2;
}
#endif


#ifdef AMBIENT_LIGHT_SENSOR
void PRSSetAmbientlight( void )
{
   sens_TSL2550Sample *light; 
   light = sens_SSimpSample_add_light(&s);

   AmbientLightSensorPrepare();
   AmbientLightVisibleSensorGet( &light->daylight );
   AmbientLightIRSensorGet( &light->infrared );
}
#else
void PRSSetAmbientlight( void ) {
   sens_TSL2550Sample *light; 
   light = sens_SSimpSample_add_light(&s);

   light->daylight=2010 ;
   light->infrared= 23;
}
#endif

#ifdef FORCE_SENSOR
void PRSSetForce( void )
{
   sens_FSR152Sample *force;
   force = sens_SSimpSample_add_force(&s);
   ForceSensorPrepare();
   ForceSensorGet( &force->value );
}
#else
void PRSSetForce( void ) {
   sens_FSR152Sample *force;
   force = sens_SSimpSample_add_force(&s);
   force->value=100;
}
#endif

#ifdef TEMPERATURE_SENSOR
void PRSSetTemp( void )
{
   sens_TC74Sample *temp;
   temp = sens_SSimpSample_add_temperature(&s);
   TemperatureSensorPrepare();
   TemperatureSensorGet( &temp->value );
}
#else
void PRSSetTemp( void ) {
   sens_TC74Sample *temp;
   temp = sens_SSimpSample_add_temperature(&s);
   temp->value=42;
}
#endif

void PRSAddSensorValueToSend( unsigned int sensor )
{
   //BuzzerTone(TONE_C4,100);

   switch( sensor )
   {
      case PRS_ACCL:          
         PRSSetAccl(); 
         break;
      case PRS_AUDIO:         
         PRSSetAudio();
         break;
      case PRS_AMBIENTLIGHT:  
         PRSSetAmbientlight();
         break;
      case PRS_FORCE:         
         PRSSetForce();
         break;
      case PRS_TEMP:       
         PRSSetTemp();
         break;
      default:
         break;
   }
}
