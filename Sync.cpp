#include "Sync.h"

Sync::Sync(Onset* onsets, Click* clk){
  _ons = onsets;
  _clk = clk;
  _is128 = false;
  _isTwoBars = false;
  _averageGrooveThreshold = 5;
  _averageGrooveStability = 12;
  _lSyncKick44[0] = _lSyncKick44[8] = 1;              // 1    // LSYNC for 4/4
  _lSyncKick44[1] = _lSyncKick44[9] = 0.1;            // and
  _lSyncKick44[2] = _lSyncKick44[10] = 1;             // 2
  _lSyncKick44[3] = _lSyncKick44[11] = 0.1;           // and
  _lSyncKick44[4] = _lSyncKick44[12] = 1;             // 3  
  _lSyncKick44[5] = _lSyncKick44[13] = 0.1;           // and
  _lSyncKick44[6] = _lSyncKick44[14] = 1;             // 4
  _lSyncKick44[7] = _lSyncKick44[15] = 0.1;           // and
  
  _lSyncSnare44[0] = _lSyncSnare44[8] = 1;            // 1
  _lSyncSnare44[1] = _lSyncSnare44[9] = 0.1;          // and
  _lSyncSnare44[2] = _lSyncSnare44[10] = 1;           // 2
  _lSyncSnare44[3] = _lSyncSnare44[11] = 0.1;         // and
  _lSyncSnare44[4] = _lSyncSnare44[12] = 1;           // 3
  _lSyncSnare44[5] = _lSyncSnare44[13] = 0.1;         // and
  _lSyncSnare44[6] = _lSyncSnare44[14] = 1;           // 4
  _lSyncSnare44[7] = _lSyncSnare44[15] = 0.1;         // and

  _lSyncKick128[0] = _lSyncKick128[12] = 1;             // 1    // LSYNC for 12/8
  _lSyncKick128[1] = _lSyncKick128[13] = 0.1;           // and
  _lSyncKick128[2] = _lSyncKick128[14] = 0.1;           // and
  _lSyncKick128[3] = _lSyncKick128[15] = 1;             // 2
  _lSyncKick128[4] = _lSyncKick128[16] = 0.1;           // and 
  _lSyncKick128[5] = _lSyncKick128[17] = 0.1;           // and
  _lSyncKick128[6] = _lSyncKick128[18] = 1;             // 3
  _lSyncKick128[7] = _lSyncKick128[19] = 0.1;           // and
  _lSyncKick128[8] = _lSyncKick128[20] = 0.1;           // and
  _lSyncKick128[9] = _lSyncKick128[21] = 1;             // 4
  _lSyncKick128[10] = _lSyncKick128[22] = 0.1;          // and
  _lSyncKick128[11] = _lSyncKick128[23] = 0.1;          // and
  
  _lSyncSnare128[0] = _lSyncSnare128[12] = 1;           // 1
  _lSyncSnare128[1] = _lSyncSnare128[13] = 0.1;         // and
  _lSyncSnare128[2] = _lSyncSnare128[14] = 0.1;         // and
  _lSyncSnare128[3] = _lSyncSnare128[15] = 1;           // 2
  _lSyncSnare128[4] = _lSyncSnare128[16] = 0.1;         // and
  _lSyncSnare128[5] = _lSyncSnare128[17] = 0.1;         // and
  _lSyncSnare128[6] = _lSyncSnare128[18] = 1;           // 3
  _lSyncSnare128[7] = _lSyncSnare128[19] = 0.1;         // and
  _lSyncSnare128[8] = _lSyncSnare128[20] = 0.1;         // and
  _lSyncSnare128[9] = _lSyncSnare128[21] = 1;           // 4
  _lSyncSnare128[10] = _lSyncSnare128[22] = 0.1;        // and
  _lSyncSnare128[11] = _lSyncSnare128[23] = 0.1;        // and
}

void Sync::initializeSync(){
  _thetaSync = 0.80;
  _beta = 0.6;
  _sigmaSync = _clk->tau / 20;
  
  for(int i=0; i<24; i++){
    _accuracyOfLastSync[i] = 0;
  }
  _lastAccuracyOfCurrentLayer = 0;
  
  _lastBarPosition16th = 0;
  _hasKick = 0;
  _hasSnare = 0;
  _lastRecovery = 0;
  for(int i=0; i<48; i++){
    _averageKickGroove[i] = 0;
    _averageSnareGroove[i] = 0;
    _storedKickGroove[i] = 0;
    _storedSnareGroove[i] = 0;
    _currentKickGroove[i] = 0;
    _currentSnareGroove[i] = 0;
  }
}

void Sync::setSwingMode(bool is128){
  if(is128){
    _lSyncKick = _lSyncKick128;
    _lSyncSnare = _lSyncSnare128;
  }else{
    _lSyncKick = _lSyncKick44;
    _lSyncSnare = _lSyncSnare44;
  }
  _is128 = is128;
  setTwoBarsLength(_isTwoBars);       
}

void Sync::setTwoBarsLength(bool isTwoBars){
  if(_is128){
    _grooveLength = 24 + (24 * isTwoBars);
    _allowedGrooveDifference = 21 + (21 * isTwoBars);
    _howManyHits =  2 + (2 * isTwoBars);
  }
  else{
    _grooveLength = 16 + (16 * isTwoBars);
    _allowedGrooveDifference = 14 + (14 * isTwoBars);
    _howManyHits = 2 + (2 * isTwoBars);
  }
  _isTwoBars = isTwoBars;
}

void Sync::updateSync(){
  float accuracy;
  float gaussian;
  float lSyncCurrent;
  long deltaTauSync = 0;
  int thisLayer = _clk->getCurrentLayer();                                    // layer of this onset
  long error = (_ons->getCurrentOnset() - _clk->expectedBeat);               // calculate error
  int currentBarPosition = _clk->barPosition;                                 // this bar position
  
  if(_ons->isSnare[_ons->currentOnsetIndex]){                                 // if it is a snare
    lSyncCurrent = _lSyncSnare[currentBarPosition];                                // get the correct value for lSync
    _hasSnare = true;
  }else{                                                                      // if it is a kick
    lSyncCurrent = _lSyncKick[currentBarPosition];                                // get the appropriate value for lSync
    _hasKick = true;                                            
  }   

  int thirdOfTau = (_clk->tau) / 3;                                           // calculate the value for the notch then:
  if(_sigmaSync < thirdOfTau && abs(error) > thirdOfTau){                     // if sigma is sufficiently low and the hit 
    _ons->setCurrentOnsetAs16th(false);                                       // falls into the notch mark it as a 16th
    return;
  }
  
  int twoSigmaSquared = -2 * pow(_sigmaSync,2);                               // calculate accuracy for the current onset
  gaussian = exp(pow(error,2) / twoSigmaSquared);                             
  accuracy = gaussian * lSyncCurrent;                                         
  if(accuracy > _thetaSync){                                                  // if accuracy is higher than the threshold
    deltaTauSync = ((gaussian + _beta) / (_beta + 1)) * accuracy * error;     // calculate the value of deltaTauSync
    if(thisLayer < _currentHigherLayer){                                      
      if(accuracy > _lastAccuracyOfCurrentLayer){                             // if the current layer is lower than the higher layer
        _accuracyOfLastSync[currentBarPosition] = accuracy;                   // but the accuracy is higher than the last synced
        _clk->setDeltaTauSync(deltaTauSync);                                  // then sync to this onset
      }
    }else{                                                                    // if the current layer is higher than the higher layer
      if(accuracy > _accuracyOfLastSync[currentBarPosition]){                 // and if the accuracy is higher than than the last synced
        _accuracyOfLastSync[currentBarPosition] = accuracy;                   // (it can be a new onset near the latest and more precise)
        _clk->setDeltaTauSync(deltaTauSync);                                  // then sync to this onset
      }
      if(accuracy >= _thetaSync + 0.1 && thisLayer >= _currentHigherLayer){   // if accuracy is higher than the threshold + headroom                                         
        _thetaSync = _thetaSync + (0.3 * (accuracy - _thetaSync - 0.1));      // change parameters (raise threshold, narrow window)
        _sigmaSync = max((_clk->tau) / 20,_sigmaSync * 
                  (_narrowWindowRatio + ((0.7 * lSyncCurrent) - accuracy)));
      }
    }
  }else{
    if(thisLayer >= _currentHigherLayer){                                     // if accuracy is lower than the threshold then change
      _thetaSync = 0.6 * _thetaSync;                                          // parameters (lower threshold, expand window)
      _sigmaSync = max((_clk->tau) / 20,_sigmaSync * 
                  (_expandWindowRatio + ((0.7 * lSyncCurrent) - accuracy)));
    }
  }
}

void Sync::updateLayerAndGroove(){
  
  long currentMillis = millis();                                              // LAYER FUNCTION
  if(_lastBarPosition != _clk->barPosition){                                  // whenever a bar position has passed do:

    if(_accuracyOfLastSync[_lastBarPosition] == 0){                           // if last bar position has no onset 
      if(_lastLayerOfBarPosition == _currentHigherLayer){                     // and current layer is == higher layer then
        _currentHigherLayer = max(0,_currentHigherLayer - 1);                 // decrease current higher layer
      }
    }else{                                                                    // else if an onset has been detected and synced
      if(_lastLayerOfBarPosition >= _currentHigherLayer){                     // and the current layer is >= than the higher layer
        _lastAccuracyOfCurrentLayer = _accuracyOfLastSync[_lastBarPosition];  // sets the current accuracy as global accuracy for the layer
        _currentHigherLayer = _lastLayerOfBarPosition;                        // and update the layer
      }
    }
    _accuracyOfLastSync[_lastBarPosition] = 0;                                // reset the accuracy value of the sync onset for last bar position
    _lastBarPosition = _clk->barPosition;                                     // update the bar position
    _lastLayerOfBarPosition = _clk->getCurrentLayer();                        // update layer of current bar position
  }

  if(_lastBarPosition16th != _clk->barPosition16th % _grooveLength){                        // RECOVERY FUNCTION
    
    _updateGrooveRepresentation();                                                          // update the groove representations

    /*for(int i=0; i<_grooveLength; i++){                                                     
      Serial.print(_storedKickGroove[i]); //_storedKickGroove[i]
    }
    Serial.print(" ");
    for(int i=0; i<_grooveLength; i++){
      Serial.print(_currentKickGroove[i]);//_currentKickGroove[i]
    }
    Serial.println();*/

    if((currentMillis - _lastRecovery) > (_clk->tau) * _grooveLength){                      // if it is time for a new resync:  
      if(_checkAndCallRecovery()){                                                          // check for the recovery and if it is
        _lastRecovery = currentMillis;                                                      // called, set the new time
      }
    }
  }
}

void Sync::_updateGrooveRepresentation(){
  _currentKickGroove[_lastBarPosition16th] = _hasKick;                                      // update the just passed 16th of the current  
  _currentSnareGroove[_lastBarPosition16th] = _hasSnare;                                  // groove for kick and snare
  
  if(_hasKick){                                                                           // update the just passed 16th of the 
    if(_averageKickGroove[_lastBarPosition16th] < _averageGrooveStability){               // average groove for kick
      _averageKickGroove[_lastBarPosition16th]++;
    }
  }else{
    if(_averageKickGroove[_lastBarPosition16th] > 0){  
      _averageKickGroove[_lastBarPosition16th]--;
    }
  }
  _storedKickGroove[_lastBarPosition16th] =                                               // update the just passed 16th of the
    _averageKickGroove[_lastBarPosition16th] > _averageGrooveThreshold;                   // stored groove for kick
  
  if(_hasSnare){                                                                          // update the just passed 16th of the 
    if(_averageSnareGroove[_lastBarPosition16th] < _averageGrooveStability){              // average groove for snare
      _averageSnareGroove[_lastBarPosition16th]++;
    }
  }else{
    if(_averageSnareGroove[_lastBarPosition16th] > 0){
      _averageSnareGroove[_lastBarPosition16th]--;
    }
  }
  _storedSnareGroove[_lastBarPosition16th] =                                              // update the just passed 16th of the stored
    _averageSnareGroove[_lastBarPosition16th] > _averageGrooveThreshold;                  // groove for snare
    
  _hasKick = false;                                                                       // reset the variables for the groove tracking
  _hasSnare = false;
  _lastBarPosition16th = _clk->barPosition16th % _grooveLength;
}

bool Sync::_checkAndCallRecovery(){
  int checkIfAfterKick = 0;                                                                             
  int checkIfAfterSnare = 0;                                                                      
  int checkIfBeforeKick = 0;
  int checkIfBeforeSnare = 0;
  int checkIfAfterKick2 = 0;                                                                             
  int checkIfAfterSnare2 = 0;                                                                      
  int checkIfBeforeKick2 = 0;
  int checkIfBeforeSnare2 = 0;
  int countHowManyKick = 0;
  int countHowManySnare = 0;
  for (int i = 0; i < _grooveLength; i++){
        if (_storedKickGroove[(i + 1) % _grooveLength] == _currentKickGroove[i]){                     // count if kick is after one 16th
          checkIfAfterKick++;
        }
        if (_storedKickGroove[(i + _grooveLength - 1) % _grooveLength] == _currentKickGroove[i]){     // count if kick is before one 16th
          checkIfBeforeKick++;
        }
        if (_storedSnareGroove[(i + 1) % _grooveLength] == _currentSnareGroove[i]){                   // count if snare is after one 16th
          checkIfAfterSnare++;
        }
        if (_storedSnareGroove[(i + _grooveLength - 1) % _grooveLength] == _currentSnareGroove[i]){   // count if snare is before one 16th
          checkIfBeforeSnare++;
        }
        
        if (_storedKickGroove[(i + 2) % _grooveLength] == _currentKickGroove[i]){                     // count if kick is after two 16th
          checkIfAfterKick2++;
        }
        if (_storedKickGroove[(i + _grooveLength - 2) % _grooveLength] == _currentKickGroove[i]){     // count if kick is before two 16th
          checkIfBeforeKick2++;
        }
        if (_storedSnareGroove[(i + 2) % _grooveLength] == _currentSnareGroove[i]){                   // count if snare is after two 16th
          checkIfAfterSnare2++;
        }
        if (_storedSnareGroove[(i + _grooveLength - 2) % _grooveLength] == _currentSnareGroove[i]){   // count if snare is before two 16th
          checkIfBeforeSnare2++;
        }
        
        countHowManyKick += _currentKickGroove[i];                                      // count how many times there is kick
        countHowManySnare += _currentSnareGroove[i];        
      }

    /*Serial.print("KICKAfter");Serial.print(checkIfAfterKick);
    Serial.print("SnareAfter");Serial.print(checkIfAfterSnare);
          Serial.print("KICKbef");Serial.print(checkIfBeforeKick);
    Serial.print("Snarebef");Serial.print(checkIfBeforeSnare);*/
      if(countHowManyKick < _howManyHits || countHowManySnare < _howManyHits){      // if the groove is not all rests (not too much zeros)
      }else{
        if(checkIfAfterKick > _allowedGrooveDifference &&                         // if the groove is one 16th after 
          checkIfAfterSnare > _allowedGrooveDifference){                          // call recovery
            //Serial.println("after1");
            _clk->setDeltaTauRecovery(-(_clk->tau)/2);
            digitalWrite(21,HIGH); // xxx
            return true;
        }else if(checkIfBeforeKick > _allowedGrooveDifference &&                  // if the groove is one 16th before 
          checkIfBeforeSnare > _allowedGrooveDifference){                         // call recovery
            //Serial.println("before1");
            _clk->setDeltaTauRecovery((_clk->tau)/2);
            digitalWrite(21,HIGH); // xxx
            return true;
        }else if(checkIfAfterKick2 > _allowedGrooveDifference &&                   // if the groove is two 16th after 
          checkIfAfterSnare2 > _allowedGrooveDifference){                          // call recovery
            //Serial.println("after1");
            _clk->setDeltaTauRecovery(-(_clk->tau)/2);
            digitalWrite(22,HIGH); // xxx
            return true;
        }else if(checkIfBeforeKick2 > _allowedGrooveDifference &&                  // if the groove is two 16th before 
          checkIfBeforeSnare2 > _allowedGrooveDifference){                         // call recovery
            //Serial.println("before1");
            _clk->setDeltaTauRecovery((_clk->tau)/2);
            digitalWrite(22,HIGH); // xxx
            return true;
        }
      }
      return false;
}
