
# Botul de sah DucaPowr

0. Trebuie sa dea la comanda citate memorabile.
1. Trebuie sa primeasca inputul de la xboard.
2. Trebuie sa scrie/raspunda catre xboard.
3. Trebuie sa stocheze pozitiile pieselor pe tabla de sah (vom defini asta ca
    fiind starea tablei).
4. Trebuie sa decida mutari pentru ambii jucatori, in functie de ce i se cere.
5. Trebuie sa decida mutarile intr-un anumit timp limita.

6. Vom spune ca botul "gandeste" atunci cand computeaza pozitii.
7. Trebuie sa poata gandi pentru ambele parti.
8. Trebuie sa sa gandeasca in timp ce oponentul gandeste. Adica sa se
    foloseasca la maxim de timpul limita.
9. Trebuie sa se opreasa din gandit odata ce oponentul face o mutare.

10. Botul este reprezentat de clasa Chess.

**Clasa Chess:**

	- trebuie sa stocheze starea tablei.
	- trebuie sa stie modul in care piesele se pot muta; reguli de atac;
        reguli de promovare; reguli pentru rocada; conditii de castig etc.
    - trebuie sa gandeasca.
	- trebuie sa aiba o metoda care actualizeaza starea tablei. (
        care e apelata atunci cand e facuta o mutare).
	- trebuie sa aiba o metoda de gandire. Aceasta trebuie sa
    aplice un algoritm pentru a calcula mutarea optima. Problema este ca
    aceasta metoda trebuie sa aiba un timp limita. Botul nu poate gandi la 
    nesfarsit. Astfel, algoritmul trebuie sa permita conditii de oprire.
	- trebuie sa aiba o metoda care returneaza o mutare in urma
    gandirii. (P.S.: daca mutarea e valida, atunci trebuie actualizata starea 
    tablei.)

## Resources
- [3-check chess](https://www.chess.com/terms/3-check-chess)
- [3-check traps](https://www.chess.com/article/view/the-3-check-traps-you-need-to-know)
- [3-check openings](https://www.chess.com/forum/view/chess960-chess-variants/3-checks-best-openings-1)
- [chessprogramming: a very resourceful site for chess engines](https://www.chessprogramming.org/Main_Page)
- [A chess engine that can play 3-check chess](https://github.com/ianfab/Fairy-Stockfish)
- [Stockfish: a very powerful chess engine](https://github.com/ddugovic/Stockfish)
- [How do computers play three-check chess?](https://chess.stackexchange.com/questions/18903/how-do-computers-play-three-check-chess)
- [How Stockfish works](http://rin.io/chess-engine/)

