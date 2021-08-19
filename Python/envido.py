from itertools import combinations
import random
class Envido():
    """Conjunto de funciones relacionadas al envido."""

    def __init__(self):
        numeros = ("As", "2", "3", "4",
        "5", "6", "7", "10", "11", "12")
        palos = ("Espada", "Basto", "Oro", "Copa")
        self.mazo = set()

        for n in numeros:
            for p in palos:
                self.mazo.add(n + " de " + p)

    def calc_envido(self, cartas):
        """Calcula el envido dadas 2 cartas"""
        #si es un set
        if type(cartas) == type(set()):
            cartas = list(cartas)

        #guardo el valor numerico de las cartas
        if cartas[0].split()[0] == "As":
            c1 = 1
        else:
            c1 = int(cartas[0].split()[0])

        if cartas[1].split()[0] == "As":
            c2 = 1
        else:
            c2 = int(cartas[1].split()[0])

        sotas = (10, 11, 12)
        #si las cartas son del mismo palo
        if cartas[0].split()[2] == cartas[1].split()[2]:
            #si ninguna es una sota
            if (not c1 in sotas) and (not c2 in sotas):
                return c1 + c2 + 20
            elif c1 in sotas:
                if c2 in sotas:
                    return 20
                else:
                    return c2 + 20
            elif c2 in sotas:
                if c1 in sotas:
                    return 20
                else:
                    return c1 + 20
        #si no son del mismo palo
        else:
            #si ninguna es una sota
            if (not c1 in sotas) and (not c2 in sotas):
                return max(c1, c2)
            elif c1 in sotas:
                if c2 in sotas:
                    return 0
                else:
                    return c2
            elif c2 in sotas:
                if c1 in sotas:
                    return 0
                else:
                    return c1

    def quien_gana_envido(self, mi_mano, mano_adv, soy_mano):
        """Retorna True si mi_mano le gana el envido a mano_adv
        y False en caso contrario."""

        #la funcion funciona con 2 o 3 cartas por mano

        if len(mi_mano) == 3:
            #calcula las dos cartas con mas envido de mi_mano
            e = dict()
            for comb in combinations(mi_mano, 2):
                e[comb] = self.calc_envido(comb)
            mi_envido = max(e.values())
        else:
            mi_envido = self.calc_envido(mi_mano)
        
        #lo mismo con mano_adv
        if len(mano_adv) == 3:
            e = dict()
            for comb in combinations(mano_adv, 2):
                e[comb] = self.calc_envido(comb)
            envido_adv = max(e.values())
        else:
            envido_adv = self.calc_envido(mano_adv)

        #veo quien tiene mas envido
        if mi_envido > envido_adv:
            return True
        elif mi_envido < envido_adv:
            return False
        elif mi_envido == envido_adv: #en caso de empate gana quien es mano
            if soy_mano:
                return True
            else:
                return False

    def prob_ganar_envido(self, mi_mano, soy_mano, carta_adv = None):
        """Retorna la probabilidad de ganar el envido dada una determinada
        mano y si se es o no mano."""

        prob = 0.0

        #si no se conoce ninguna carta del adversario
        if carta_adv == None:
            for comb in combinations(self.mazo - mi_mano, 3):
                if self.quien_gana_envido (mi_mano, comb, soy_mano) == True:
                    prob += 1.0
            prob /= 7770
        #si, en cambio, se conoce una
        else:
            for comb in combinations(self.mazo - mi_mano, 3):
                if carta_adv in comb:
                    if self.quien_gana_envido (mi_mano, comb, soy_mano) == True:
                        prob += 1.0
            prob /= 630

        #redondeo
        if(prob > 1.0):
            prob = 1.0

        return prob

def test():
    mi_mano = {"2 de Basto", "As de Oro", "10 de Oro"}
    env = Envido()
    soy_mano = True
    print("Proba de ganar envido dado el mazo", mi_mano, ":\n",
    env.prob_ganar_envido(mi_mano, soy_mano))

if __name__ == "__main__":
    test()