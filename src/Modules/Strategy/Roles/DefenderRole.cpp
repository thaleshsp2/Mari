#include "DefenderRole.h"
#include "Core/Utils/Math.h"
#include "Core/Utils/RelativeCoord.h"
#include "Core/Utils/RobotDefs.h"
#include "Core/Utils/CartesianCoord.h"

DefenderRole::DefenderRole(SpellBook *spellBook) : InnerModule(spellBook)
{

    onBall = false;
    onPosition = false;
    contPerdido = 0;
    scanPitch = 0;
    conta = 0;
    conta2 = 0;
    kick = 0;
    kickLeft = false;
}
DefenderRole::~DefenderRole()
{
}
void DefenderRole::Tick(float ellapsedTime, const SensorValues &sensor)
{
    spellBook->strategy.MoveHead = false;
    CartesianCoord coord;
    RelativeCoord rr;
    if ((spellBook->strategy.GameState == STATE_READY || spellBook->strategy.GameState == STATE_PLAYING) &&
        !onPosition)
    {

        if (spellBook->perception.vision.localization.Enabled)
        {
            contPerdido = 0;
            spellBook->strategy.WalkForward = true;
            spellBook->strategy.TargetX = 0.0f;
            spellBook->strategy.TargetY = 0.0f;
            spellBook->strategy.TargetTheta = 0;

            CartesianCoord current(spellBook->perception.vision.localization.X, spellBook->perception.vision.localization.Y);
            CartesianCoord desired(spellBook->strategy.TargetX, spellBook->strategy.TargetY);
            if (current.distance(desired) < 0.2f)
            {
                spellBook->strategy.WalkForward = false;
                onPosition = true;
            }
        }
        else
        {
            onPosition = true;
        }
    }

    if (spellBook->strategy.GameState == STATE_PLAYING)
    {
        if(kick > 60)
        {
            spellBook->motion.Vth = 0;
            spellBook->motion.Vx = 0;
            spellBook->motion.HeadPitch = 0;
            if(spellBook->strategy.FakeKick)
            {
                spellBook->motion.Vx = 0.3f;
            }
            else
            {
                spellBook->motion.KickLeft = kickLeft;
                spellBook->motion.KickRight = !kickLeft;
            }
            kick++;
            if(kick > 100)
            {
                kick = 0;
                if(spellBook->strategy.FakeKick)
                {
                    spellBook->motion.Vx = 0;
                }
                else
                {
                    spellBook->motion.KickLeft = false;
                    spellBook->motion.KickRight = false;
                }
            }
        }
        else
        {
            if(spellBook->perception.vision.ball.BallLostCount < 8)
            {
                spellBook->motion.Vy = 0;
                if(abs(spellBook->perception.vision.ball.BallYaw) > Deg2Rad(10.0f))
                {
                    spellBook->motion.Vth = -spellBook->perception.vision.ball.BallYaw * 0.5f;
                    spellBook->motion.Vx = 0;
                    kick = 0;
                }
                else
                {
                    if(abs(spellBook->perception.vision.ball.BallYaw) > Deg2Rad(5.0f))
                        spellBook->motion.Vth = -spellBook->perception.vision.ball.BallYaw * 0.4f;
                    else
                        spellBook->motion.Vth = 0;

                    if(spellBook->perception.vision.ball.BallDistance < 0.45f)
                        spellBook->motion.HeadPitch = Deg2Rad(24.0f);
                    else if(spellBook->perception.vision.ball.BallDistance > 0.5f)
                        spellBook->motion.HeadPitch = Deg2Rad(0.0f);
                    if(spellBook->perception.vision.ball.BallDistance > 0.25f)
                    {
                        spellBook->motion.Vx = min(spellBook->perception.vision.ball.BallDistance * 0.25f, 0.25f);
                        kick = 0;
                    }
                    else
                    {
                        spellBook->motion.Vx = 0;
                        if(spellBook->motion.HeadPitch > 0)
                        {
                            kick++;
                            kickLeft = spellBook->perception.vision.ball.BallYaw < 0;
                        }
                        else
                        {
                            kick = 0;
                        }
                    }
                }
            } else 
            {
                spellBook->motion.KickRight = false;
                spellBook->motion.Vth = 0;
                spellBook->motion.Vx = 0;
                //cout << "Não entrou no IF" << endl;

                // Para -Y negativo, variar Vth=-1.6 e Vy=-0.1
                
                if(conta<550){
                    spellBook->strategy.MoveHead = false;
                    conta++;
                    spellBook->motion.Vy=0.1;
                    spellBook->motion.Vth=Deg2Rad(2.75);
                } else if(conta<650){
                    conta++;
                    spellBook->motion.Vy=0;
                    spellBook->strategy.MoveHead = true;
                } else if(conta<800 && conta2<2){
                    conta++;
                    //spellBook->motion.Vth=Deg2Rad(5.8);
                } else if(conta<900){
                    conta++;
                    spellBook->motion.Vth=0;
                    if(conta == 899){
                        conta2++;
                    }
                    if(conta2<2 && conta == 899){
                    conta = 0;
                    }   
                } else if(conta <2050) {
                    spellBook->strategy.MoveHead = false;
                    spellBook->motion.Vy = -0.1;
                    spellBook->motion.Vth = -Deg2Rad(1.6); 
                    conta++;
                } else {
                    conta = 0;
                    conta2 = 0;
                }

                cout << "Parado" << endl;
            }
        }
    }
}
