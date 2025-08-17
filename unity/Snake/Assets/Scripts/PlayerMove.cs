using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using UnityEngine.UI;
using TMPro;
using UnityEngine.SceneManagement;

public class PlayerMove : MonoBehaviour
{
    // Difficulty of the game (from GameManager)
    int difficulty = 1;

    // Current movement direction of the snake
    private Vector2 currentDirection = Vector2.right;
    // Previous direction used to prevent reversing
    private Vector2 prevDirection = Vector2.right;

    // Input vector from player
    public Vector2 moveInput;

    // Input system actions
    private InputSystem_Actions controls;

    // New position values for snake head
    private float newX;
    private float newY;

    // Coroutine for continuous snake movement
    Coroutine moveCoroutine;

    // Snake body prefab
    public GameObject body;

    // LinkedList to keep track of all snake body parts
    private LinkedList<GameObject> snakeList;

    // Temporary variable for creating new body parts
    private GameObject newBody;

    // Variables to store previous head position/rotation
    private Vector2 prevPosition;
    private Quaternion prevRotation;

    // Flag for whether snake just ate an apple
    bool ifApple = false;

    // Reference to AppleSpawner to stop spawning when game ends
    public AppleSpawner appleSpawner;

    // Sprites for head, eating head, dead head, and tail
    public Sprite head;
    public Sprite headEat;
    public Sprite headDead;
    public Sprite tail;

    // UI elements
    public TextMeshProUGUI scoreText;
    public TextMeshProUGUI gameOverText;
    public Button restartButton;

    // Player score
    int score;

    // Audio source for eating sound
    public AudioSource eatingAudio;

    // Start is called once before the first execution of Update after the MonoBehaviour is created
    void Awake()
    {
        // Get difficulty from GameManager if it exists
        if (GameManager.Instance != null)
        {
            difficulty = GameManager.Instance.difficulty;
        }

        // Setup input system
        controls = new InputSystem_Actions();
        controls.Player.Move.performed += ctx => moveInput = ctx.ReadValue<Vector2>();
        controls.Player.Move.canceled += ctx => moveInput = Vector2.zero;
    }

    void Start()
    {
        // Initialize the LinkedList for snake body parts
        snakeList = new LinkedList<GameObject>();

        // Create the first and the second body part
        newBody = Instantiate(body);
        newBody.transform.position = new Vector2(-4.5f, 0.5f);
        snakeList.AddFirst(newBody);
        newBody = Instantiate(body);
        newBody.transform.position = new Vector2(-5.5f, 0.5f);
        snakeList.AddLast(newBody);

        // Start movement coroutine
        moveCoroutine = StartCoroutine(PerformActionMove());

        // Initialize score
        score = 0;
    }

    private void OnTriggerEnter2D(Collider2D other)
    {
        // Collision with apple
        if (other.CompareTag("Apple"))
        {
            ifApple = true; // Flag that snake ate an apple
            GetComponent<SpriteRenderer>().sprite = headEat; // Change head sprite
            Destroy(other.gameObject);// Remove apple
            eatingAudio.Play(); // Play sound
            score++; // Increase score
        }

        // Collision with snake body
        if (other.CompareTag("Body"))
        {
            GetComponent<SpriteRenderer>().sprite = headDead; // Change head to dead sprite
            StopCoroutine(moveCoroutine); // Stop snake movement
            StopCoroutine(appleSpawner.appleCoroutine); // Stop spawning apples
            GameOver(); // Trigger game over UI
        }
    }

    // Coroutine to move the snake at fixed intervals
    IEnumerator PerformActionMove()
    {
        while (true)
        {
            DoActionMove(); // Move snake
            yield return new WaitForSeconds(0.3f / difficulty); // Wait based on difficulty
        }
    }

    // Moves the snake and updates body parts
    void DoActionMove()
    {
        // Set head sprite to normal
        GetComponent<SpriteRenderer>().sprite = head;

        // Store previous position/rotation for body follow
        prevPosition = transform.position;
        prevRotation = transform.rotation;
        prevDirection = currentDirection;

        // Calculate new head position
        newX = transform.position.x + currentDirection.x;
        newY = transform.position.y + currentDirection.y;

        // Wrap around screen edges
        if (newX > 8.5f) newX = -8.5f;
        else if (newX < -8.5f) newX = 8.5f;
        else if (newY > 4.5f) newY = -4.5f;
        else if (newY < -4.5f) newY = 4.5f;

        // Apply new position and rotation
        transform.position = new Vector2(newX, newY);
        transform.rotation = Quaternion.Euler(0, 0, Mathf.Atan2(currentDirection.y, currentDirection.x) * Mathf.Rad2Deg);

        // Create a new body segment at previous head position
        newBody = Instantiate(body);
        newBody.transform.position = prevPosition;
        newBody.transform.rotation = prevRotation;
        snakeList.AddFirst(newBody); // Add to front of list

        // If no apple was eaten, remove last tail segment
        if (!ifApple)
        {
            newBody = snakeList.Last.Value;
            Destroy(newBody); // Destroy last segment
            snakeList.RemoveLast(); // Remove from LinkedList

            // Update tail sprite and rotation
            snakeList.Last.Value.GetComponent<SpriteRenderer>().sprite = tail;
            snakeList.Last.Value.transform.rotation = snakeList.Last.Previous.Value.transform.rotation;
        }
        else
        {
            ifApple = false; // Reset apple flag
        }
    }

    // Enable input actions
    void OnEnable()
    {
        controls.Player.Enable();
    }

    // Disable input actions
    void OnDisable()
    {
        controls.Player.Disable();
    }

    void Update()
    {   
        // Update direction if input is valid and not reversing
        if ((moveInput.x != 0 ^ moveInput.y != 0) &&
            (moveInput.x != prevDirection.x && moveInput.y != prevDirection.y))
        {
            currentDirection = moveInput;
        }

        // Update UI score text
        scoreText.text = $"Score: {score}";
    }

    // Display Game Over UI
    void GameOver()
    {
        gameOverText.gameObject.SetActive(true);
        restartButton.gameObject.SetActive(true);
    }
}
