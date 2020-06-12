# Make wheel 2 position match wheel 1 position
# No PID yet.

from microbit import *
import ustruct

ev8_16 = 116
buf1 = bytearray([0])
buf2 = bytearray([0, 0])
buf4 = bytearray([0, 0, 0, 0])
bufEnc = bytearray([0, 3, 2, 1, 0])
bufEncOut = bytearray([0, 0, 0, 0, 0])

class tbos:
    def setMotorPower(m, p):
        global buf2
        if m == 1:
            buf2[0] = 10
        elif m == 2:
            buf2[0] = 20
        else:
            return  # no such motor
        if p > 100:
            p = 100
        if p < -100:
            p = -100
        buf2[1] = int(p)
        pin16.write_digital(0)
        spi.write(buf2)
        pin16.write_digital(1)
        return

    def playNote(k):
        global buf2
        buf2[0] = 62
        if k < 0:
            k = 0
        if k > 88:
            k = 88
        buf2[1] = int(k)
        pin16.write_digital(0)
        spi.write(buf2)
        pin16.write_digital(1)
        return

    def playFrequency(k):
        global buf4
        if k < 0:
            k = 0
        if k > 10000:
            k = 10000

        buf4[0] = 63
        buf4[1] = ev8_16
        buf4[2] = (int(k) & 0x0000ff00) >> 8
        buf4[3] = (int(k) & 0x000000ff)

        pin16.write_digital(0)
        spi.write(buf4)
        pin16.write_digital(1)
        return

    def clearEncoder(i):
        global buf2
        if i == 1:
            buf2[0] = 15
        elif i == 2:
            buf2[0] = 25
        else:
            return 0  # no such encoder

        buf2[1] = 0
        pin16.write_digital(0)
        spi.write(buf2)
        pin16.write_digital(1)
        return


    def getEncoder(i):
        global bufEnc
        if i == 1:
            bufEnc[0] = -15
        elif i == 2:
            bufEnc[0] = -25
        else:
            return 0  # no such encoder

        pin16.write_digital(0)
        spi.write_readinto(bufEnc, bufEncOut)
        pin16.write_digital(1)
        t = ustruct.unpack_from('<i', bufEncOut, 1)
        return t[0]

spi.init()

############ Add your code here.##############
display.show(Image.HAPPY)
sleep(1000)
tbos.clearEncoder(1)
tbos.clearEncoder(2)
sleep(100)
# Spin your TB2 and measure rotation around the z axis
while True:
    e1 = tbos.getEncoder(1)
    e2 = -tbos.getEncoder(2)
    ediff = e1-e2
    tbos.setMotorPower(2, ediff/50)


"""
def train_epoch(self, epoch):

        avg_loss = 0.0
        total_correct = 0.0
        avg_correct = 0.0
        total = 0.0

        ### TODO: Implement the training for one epoch
        ### How do you make sure the network is actually training?
        ### What are the 5 steps you need to do for every minibatch?
        ### Keep track of the loss AND the number of correctly clasified cells
        ### Return the average loss and the average over the number of corrrect classifications for this epoch
        ##############

            ### training loop ###

        self.net.train()
        running_loss_train = 0.0
        i = 0
        G_values = []
        S_values = []
        G_values2 = []
        S_values2 = []
        sets = 0.0
        counter = 0
        for pair_lists, scores in self.train_loader:
            pair_list = pair_lists[0]
            score = scores[0]
            if (score != score): #Nan
                 continue
            for pair_lists2, scores2 in self.train_loader:
                
                pair_list2 = pair_lists2[0]
                score2 = scores2[0]
                
                if (scores2 != scores2):
                    continue

                G = torch.tensor(0.0)
                G2 = torch.tensor(0.0)

                for net_in in pair_list:
                    reward = self.net(net_in)
                    #print("input")
                    #print(net_in.size())
                    #print("reward")
                    #print(reward)
                    G += reward.item()

                for net_in in pair_list2:
                    reward = self.net(net_in)
                    #print("input")
                    #print(net_in.size())
                    #print("reward")
                    #print(reward)
                    G2 += reward.item()
            
                #G_values.append(G)
                #G_values2.append(G2)
                #S_values.append(score)
                #S_values2.append(score2)
            
                #print(len(G_values[0]))
                #print(len(S_values[0]))
                #Make two-column vector to compare 
                #G_values = G_values
                #S_values = S_values
                #output = self.compute_comparisons(G_values)
                #lbls = self.compute_comparisons(S_values)
                #loss = Variable(self.loss_criterion(output, lbls), requires_grad = True)
                output = torch.tensor([G/(G+G2), G2/(G+G2)])
                lbls = torch.tensor([score/(score+score2), score2/(score+score2)])
                loss = self.loss_criterion(output, lbls)
                loss = Variable(loss, requires_grad = True)
                print(loss.grad)
                loss.backward()
                self.optimizer.step()
                running_loss_train += loss.item()
                sets += 1
                #overall, correct = self.numCorrect(output, lbls)
                overall, correct = 1, 0
                total += overall
                total_correct += correct
                optimizer.zero_grad()
                i = 0

        running_loss_train /= sets 
        avg_correct = total_correct / total

        return running_loss_train, avg_correct

"""